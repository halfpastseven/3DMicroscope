#include "analyzer.h"
#include <QDialog>
#include <QMessageBox>
#include <cmath>
#include <QPoint>
#include <cstdio> //For debug
#include <iostream> //For debug
#include <ctime>
#include <QDataStream>
#include <QFile>
#include <QImage>

analyzer::analyzer()
{
    setRadius(3);
}

analyzer::~analyzer() {
    
}

void analyzer::setImage(const QImage *sourceImage)
{
    Source = sourceImage;
}

int analyzer::analyzeGrad()
{
    StopAnalysis = 0;
    //time_t startTime = time(NULL);

    /** For this analysis, first define a "spot"
     *  A spot is a group of pixels enclosed by a circle of Radius
     *  For the pixels, do all the channels 
     *  SSR = [Sigma] R^2 * N - ([Sigma] R)^2
     *  SSG = [Sigma] G^2 * N - ([Sigma] G)^2
     *  SSB = [Sigma] B^2 * N - ([Sigma] B)^2
     * 
     **/
    
    QVector <QPoint> Spot;
    QVector <QPoint> SpotEdgeLt;
    QVector <QPoint> SpotEdgeRt;
    maxSharpness = 0;
        
    //Define the spot region for analysis

    if (Margin*2+1 >=Source->width() || Margin*2+1 >=Source->height()) return 0;
    ResultBuffer = QVector <QVector <int32_t> > 
            (Source->height()-2*Margin,QVector <int32_t> (Source->width()-2*Margin,0));
    
    for (int iY = -Margin; iY<=Margin; iY++){
        //Assess each pixel in this square and assess vertical edges
        bool PreviousIn = 0;
        for (int iX = -Margin; iX<=Margin; iX++){
            bool CurrentIn=0;
            if ((iX*iX + iY*iY) <(Radius*Radius + 1e-5)){
                Spot.push_back(QPoint(iX,iY));
                CurrentIn = 1;
            }

            if (!PreviousIn && CurrentIn)  SpotEdgeLt.push_back(QPoint(iX, iY));
            if (PreviousIn && !CurrentIn)  SpotEdgeRt.push_back(QPoint(iX-1, iY));
            if (iX == Margin && CurrentIn) SpotEdgeRt.push_back(QPoint(iX, iY));
            
            if (CurrentIn) PreviousIn = 1;
            else PreviousIn = 0;
        }
    }
    
    SpotSize = Spot.size();
    
    /** Now use the spot circle to scan the picture for gradient**/
    

    /// To save time, store the R2, G2, B2 (squares) in a buffer (squareBuffer)
    /// Size of the buffer is (2*Margin+1)*Width*[R,G,B]
    QVector <QVector <QVector <int> > > squareBuffer (Source->height());
    
    //Initialize Buffer2 by filling with 2*Margin+1 rows of data
    
    for (int iRow =0; iRow<2*Margin+1; iRow++){
        squareBuffer[iRow] = QVector <QVector<int> > (Source->width(),
                                                 QVector <int> (3,0));
        for (int iCol = 0; iCol<Source->width(); iCol++){
            squareBuffer[iRow][iCol][0] = R(iCol,iRow)*R(iCol,iRow);
            squareBuffer[iRow][iCol][1] = G(iCol,iRow)*G(iCol,iRow);
            squareBuffer[iRow][iCol][2] = B(iCol,iRow)*B(iCol,iRow);
        }
    }
    
    ///Start scanning
    for (int iY = Margin; iY< Source->height()-Margin; iY++){
        if (StopAnalysis) return -1;
        int sumR  =0;
        int sumR2 =0;
        int sumG  =0;
        int sumG2 =0;
        int sumB  =0;
        int sumB2 =0;
        
        //Evaluate the start of the line (Horizontal)
        for (int spotPix = 0; spotPix <Spot.size(); spotPix++){
            
            int currentX = Spot[spotPix].x()+Margin;
            int currentY = Spot[spotPix].y()+iY;
            
            sumR += R(currentX, currentY);
            sumR2+= squareBuffer[currentY][currentX][0];
            
            sumG += G(currentX, currentY);
            sumG2+= squareBuffer[currentY][currentX][1];
            
            sumB += B(currentX, currentY);
            sumB2+= squareBuffer[currentY][currentX][2];
        }
        int32_t Result = calculatePixelSharpness (sumR, sumR2, sumG, sumG2, sumB, sumB2);
        ResultBuffer[iY-Margin][0] = Result;
        if (Result > maxSharpness) maxSharpness = Result;

        //Finish the rest of the line
        for (int iX = Margin+1;iX< Source->width()-Margin; iX++){

            //First, calculate the left edge pixels of the spot (SpotEdgeLt) and subtract
            for (int EdgePix=0; EdgePix<SpotEdgeLt.size();EdgePix++){
                int currentX = SpotEdgeLt[EdgePix].x()+iX-1;
                int currentY = SpotEdgeLt[EdgePix].y()+iY;
                
                sumR -= R(currentX, currentY);
                sumR2-= squareBuffer[currentY][currentX][0];;
                
                sumG -= G(currentX, currentY);
                sumG2-= squareBuffer[currentY][currentX][1];
                
                sumB -= B(currentX, currentY);
                sumB2-= squareBuffer[currentY][currentX][2];
            }
            //Then, calculate the right edge pixels of the spot (SpotEdgeRt) and add
            for (int EdgePix=0; EdgePix<SpotEdgeRt.size();EdgePix++){
                int currentX = SpotEdgeRt[EdgePix].x()+iX;
                int currentY = SpotEdgeRt[EdgePix].y()+iY;
                
                sumR += R(currentX, currentY);
                sumR2+= squareBuffer[currentY][currentX][0];
                
                sumG += G(currentX, currentY);
                sumG2+= squareBuffer[currentY][currentX][1];
                
                sumB += B(currentX, currentY);
                sumB2+= squareBuffer[currentY][currentX][2];
                
            }
            
            Result = calculatePixelSharpness 
                    (sumR, sumR2, sumG, sumG2, sumB, sumB2);
            ResultBuffer[iY-Margin][iX-Margin] = Result;
            if (Result > maxSharpness) maxSharpness = Result;
        }
    
        //Move the buffer down
        squareBuffer[iY-Margin].clear(); //Clear the row in the back
        if (iY < Source->height()-Margin-1) { //Avoid segmentation fault
            int iRow=iY+Margin+1;
            squareBuffer[iRow] = QVector<QVector<int> > (Source->width(),QVector<int>(3));
            for (int iCol = 0; iCol<Source->width(); iCol++){ //Generate new row in the front
                squareBuffer[iRow][iCol][0] = R(iCol,iRow)*R(iCol,iRow);
                squareBuffer[iRow][iCol][1] = G(iCol,iRow)*G(iCol,iRow);
                squareBuffer[iRow][iCol][2] = B(iCol,iRow)*B(iCol,iRow);
            }
        }
    }
    return 0;
}
