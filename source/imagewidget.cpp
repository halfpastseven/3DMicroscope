#include "imagewidget.h"
#include <QRect>
#include <QPainter>

imageWidget::imageWidget(QWidget *parent, int defaultWidth, 
                         int defaultHeight, const QImage* imageSource) :
    QWidget(parent), Image(imageSource), 
    defaultW(defaultWidth),defaultH(defaultHeight) {    
    setFixedSize(defaultW+2,defaultH+2);
}

imageWidget::~imageWidget(){

}

void imageWidget::paintEvent(QPaintEvent *){
    
    if (Image==NULL) return;
    if (Image->isNull()) return;
    
    QPainter painter (this);
    
    double defaultAR = defaultW/defaultH;
    double imageAR = Image->width()/double (Image->height());
    int H = defaultH;
    int W = defaultW;

    if (imageAR>defaultAR){ //Adjust to image aspect ratio
        H = W/imageAR;
    }    
    else {
        W = H*imageAR;
    }
    
    QRect ImageRect ((width()-W)/2,(height()-H)/2, W, H);

    painter.drawImage(ImageRect,*Image);
    painter.drawRect(ImageRect);
}
