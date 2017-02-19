#ifndef ANALYZER_H
#define ANALYZER_H

#include <QImage>
#include <QVector>
#include <QDataStream>

class analyzer
{

public:
    analyzer() ;
    ~analyzer ();
    
    /* Basic functions */
    void setImage (const QImage* sourceImage);
    int  analyzeGrad();
    int  analyze3D(const QStringList &Filenames, QVector<QVector<int> > &zImage, QImage &compositeImage);
    
    /* Getters and setters */
    inline const QVector <QVector <int32_t> >& getResult() const {
        return ResultBuffer;
    }
    
    inline float getRadius() const {return Radius;}
    void setRadius(float value){
        Radius = value;
        if (Radius<0) Radius = -Radius;
        if (Radius<1) Radius = 1;
        Margin = floor (Radius);
    }
       
    inline int getMargin() const {return Margin;}
    int getSpotSize() const {return SpotSize;}
    
private:

    const QImage* Source;
    float Radius;
    int   Margin;
    int   SpotSize;
    int   maxSharpness;
    int   imageW;
    int   imageH;
    bool  StopAnalysis;
    QVector <QVector <int32_t> > ResultBuffer;
    
    /* Private methods */
    // Tell the RGB of pixels
    inline int R (int x, int y)const{return qRed  (((QRgb*) Source->constScanLine(y))[x]);}
    inline int G (int x, int y)const{return qGreen(((QRgb*) Source->constScanLine(y))[x]);}
    inline int B (int x, int y)const{return qBlue (((QRgb*) Source->constScanLine(y))[x]);}
    // Calculate the pixel sharpness
    inline int32_t calculatePixelSharpness (
            int sumR, int sumR2,int sumG, int sumG2,int sumB, int sumB2){
        return (SpotSize*(sumR2+sumG2+sumB2)-sumR*sumR-sumG*sumG-sumB*sumB);
    }

signals:
};

#endif // ANALYZER_H
