#include "thumbnailseries.h"
#include <QPainter>
#include <QMouseEvent>

thumbnailSeries::thumbnailSeries(QWidget *parent,const QVector <QImage>* sourceImages) : 
    QWidget(parent), Images(sourceImages)
{
    thumbLayout = new QHBoxLayout;  
    setLayout(thumbLayout);
    show();
}

void thumbnailSeries::paintEvent(QPaintEvent *)
{
    if (thumbLayout->count() && activeImage>=0 && activeImage<thumbLayout->count()){
        //Draw a rectangle around the active image
        QPainter rectPainter (this);
        const int BorderSize = 2;
        rectPainter.setPen(QPen(Qt::red,BorderSize));
        
        QPoint positionOfActive =thumbLayout->itemAt(activeImage)->widget()->pos();
        rectPainter.drawRect(positionOfActive.x()-BorderSize/2,positionOfActive.y()-BorderSize/2,
                             tnSize+BorderSize,tnSize+BorderSize);  
    }
}

void thumbnailSeries::mousePressEvent(QMouseEvent* mouseEvent)
{
    if (!thumbLayout->count()) return;
    activeImage = mouseEvent->pos().x()/(tnSize+2*tnMargin);
    
    emit activeImageChanged (activeImage);
    
    update();
}

void thumbnailSeries::updateThumbnails()
{
    /** Erase all the images **/
    if (thumbLayout->count()){
        for (int iImg = thumbLayout->count()-1; iImg>=0; iImg--){
            QWidget* imageSelected = thumbLayout->itemAt(iImg)->widget();
            thumbLayout->removeWidget(imageSelected);
            delete imageSelected;
        }
    }
    
    /** Add the new images **/
    for (int iImg = 0; iImg < Images->size(); iImg++){
        imageWidget* newThumbnail  = new imageWidget(this, tnSize+2,tnSize+2,
                                                     &((*Images)[iImg]));
        thumbLayout->addWidget(newThumbnail,0,Qt::AlignLeft);
        thumbLayout->itemAt(iImg)->widget()->setFixedSize(tnSize, tnSize);
    }
    
    resize ((tnSize+tnMargin*2)*thumbLayout->count(),tnSize+2*tnMargin+10);
    
    /** Ensure the active image is within the new range **/
    if (activeImage> Images->size()-1) activeImage = Images->size()-1;
    else if (activeImage<0) activeImage = 0;
    
    update();
}
