#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QImage>

class imageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit imageWidget(QWidget *parent, int defaultWidth,int defaultHeight,                         
                         const QImage* imageSource=NULL);
    
    void setImage (const QImage* imageSource) {Image = imageSource;}
    void paintEvent(QPaintEvent*);
    ~imageWidget();
    
signals:
    
public slots:
private:
    
    const QImage* Image;
    int defaultW;
    int defaultH;
};



#endif // IMAGEWIDGET_H
