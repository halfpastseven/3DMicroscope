#ifndef THUMBNAILSERIES_H
#define THUMBNAILSERIES_H

#include "imagewidget.h"
#include <QWidget>
#include <QVector>
#include <QImage>
#include <QHBoxLayout>
#include <QPainter>

class thumbnailSeries : public QWidget
{
    Q_OBJECT
public:
    explicit thumbnailSeries(QWidget *parent, const QVector<QImage> *sourceImages);
    
    int getActiveImage() const{return activeImage;}
    void setActiveImage(int Active) {activeImage=Active;}
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent* mouseEvent);

signals:
    void activeImageChanged (int iImg);
    
private:
    
    QHBoxLayout* thumbLayout;
    const QVector <QImage>* Images;
    
    int activeImage;
    const int tnSize = 100;
    const int tnMargin = 5;
    bool imagesChanged;
    

public slots:
    void updateThumbnails();
};

#endif // THUMBNAILSERIES_H
