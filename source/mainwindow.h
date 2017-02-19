#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QDataStream>
#include <QDialog>
#include <QFile>
#include <QHBoxLayout>
#include <QImage>
#include <QMainWindow>
#include <QScrollArea>
#include <QToolBar>

#include "imagewidget.h"
#include "analyzer.h"
#include "thumbnailseries.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    /* System components */
    QStringList loadFileNames; // List of files to analyze
    QVector <QImage> origImages; // Vector of images to analyze
    QImage gradImage;  // Gradient image
    QImage compositeImage; // Composite image
    QImage depthImage;
    
    analyzer* Analyzer; // Analyzer object
    QFile*  BufferFile; // Temporary storage of results

    QVector <QVector <int> > zMatrix; //Matrix storing the layer number
    QVector <QVector <int32_t> > maxGradient; 
    
    /* Window widgets */
    QToolBar*       mainToolBar;    
    QStatusBar*     mainStatusBar;
    
    imageWidget*    origImgWidget; //Display the original image
    imageWidget*    gradImgWidget; //Display the gradient image

    QDialog*        resultsDialog;
    imageWidget*    compImgWidget; //Display the composite image
    imageWidget*    deptImgWidget; //Display the depth image
    
    QScrollArea*    thumbScrollArea;
    thumbnailSeries* Thumbnails;
    
    /* Actions */
    QAction*    actionLoadImage;
    QAction*    actionSetMag;
    QAction*    actionViewResults;
    QAction*    actionSetRad;
    QAction*    actionAnalyze;
    
    /* Widget properties */
    const char* Version = "1.0";
    int imageW;
    int imageH;
    
    int currentDisplayedImage;
    bool imagesAnalyzed;
    bool fromTopToBottom;
    int gradientThreshold;
    QString currentDirectory;
    float contrastMagnification;
    
    // constants
    const int THUMBNAILSIZE=100;
    const int DEFAULT_GRADIENTTHRESHOLD=50;

    /* Private methods */
    void updateSeries();
    void setContrast(float value) {
        if (value == contrastMagnification) return;
        contrastMagnification = value>0 ? value:1;
        activeImageUpdated(currentDisplayedImage);
    }
    void updateResults();
    
public slots:
    void loadImageSet();
    void analyzeImages ();
    void setRad();
    void setMag();
    void setGradientThreshold();
    void activeImageUpdated(int iImg);
    void aboutMe();
    void aboutQt();
    void selectTopDown  ();
    void selectBottomUp ();
    void openResultsWindow();
    void saveCompImage();
    void saveDeptImage();
};

#endif // MAINWINDOW_H
