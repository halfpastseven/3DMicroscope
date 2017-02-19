#include "mainwindow.h"
#include "thumbnailseries.h"

#include <QBuffer>
#include <QButtonGroup>
#include <QGridLayout>
#include <QGroupBox>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>
#include <QInputDialog>
#include <QStringList>
#include <QWidget>

//#define DEVELOPING
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    currentDirectory = QString (
#ifdef DEVELOPING
        "E:\\Documents\\programming\\Qt\\3dMicroscope\\SampleFiles\\GDLScaled"
#endif
        );

    
    /** Set up actions **/
    actionLoadImage = new QAction (QIcon(":/icon/open.png"),"Load images");
    actionLoadImage->setToolTip("Load new images and overwrite existing images");
    
    actionSetMag = new QAction (QIcon(":/icon/setM.png"), "Set contrast magnification");
    actionSetMag->setToolTip("Set contrast magnification for the gradient image");
    
    actionSetRad = new QAction (QIcon(":/icon/setR.png"), "Set spot radius");
    actionSetRad->setToolTip("Set the radius of the spot used to calculate sharpness");
    
    actionViewResults = new QAction (QIcon (":/icon/showResults"), "Show results");
    actionViewResults->setToolTip("Display composite image and depth image");
    
    actionAnalyze = new QAction (QIcon(":/icon/analyze.png"), "Analyze image set");
    actionAnalyze->setToolTip("Analyze the images for depth information");
    
    /** Set up menus **/
    QMenu* fileMenu = new QMenu("File");
    menuBar()->addMenu(fileMenu);
    fileMenu->addAction(actionLoadImage);
    
    QMenu* viewMenu = new QMenu("View");
    menuBar()->addMenu(viewMenu);
    viewMenu->addAction(actionSetMag);
    viewMenu->addAction(actionViewResults);
    
    QMenu* analyzeMenu = new QMenu ("Analyze");
    analyzeMenu->addAction(actionSetRad);
    analyzeMenu->addAction(actionAnalyze);

    QMenu* aboutMenu = new QMenu ("About");
    menuBar()->addMenu(aboutMenu);
    aboutMenu->addAction (QIcon(":/icon/3DMicroscope.ico"),QString("About"),this,&aboutMe);
    aboutMenu->addAction (QIcon(":/icon/qt-logo@4x.png"),QString("About Qt"),this,&aboutQt);
    
    mainToolBar = new QToolBar(this);
    addToolBar(mainToolBar);
    mainToolBar->setMovable(0);
    
    mainToolBar->addAction(actionLoadImage);
    mainToolBar->addAction(actionAnalyze);
    mainToolBar->addAction(actionSetRad);
    mainToolBar->addAction(actionSetMag);
    mainToolBar->addAction(actionViewResults);
    
    Analyzer = new analyzer();
    
    /** Set up central widget **/
    /** Top leve: Central Widget **/
    QWidget* CentralWidget = new QWidget (this);
    CentralWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    setCentralWidget(CentralWidget);

    /* Central widget layout*/
	QVBoxLayout* centralWidgetLayout = new QVBoxLayout;
    CentralWidget->setLayout(centralWidgetLayout);
	
    /** 1. Image display region**/
    QGridLayout* graphLayout = new QGridLayout ();
	centralWidgetLayout -> addLayout (graphLayout);
    
    origImgWidget = new imageWidget (this, 480, 320);
    gradImgWidget = new imageWidget (this, 480, 320,&gradImage);
    
    /** 1.1 Original Image Widget **/
    graphLayout->addWidget(origImgWidget,0,0,1,1);
    /** 1.2 Gradient Image Widget **/
    graphLayout->addWidget(gradImgWidget,0,1,1,1);
    contrastMagnification=1;

    mainStatusBar = new QStatusBar(this);
    setStatusBar(mainStatusBar);
    statusBar()->showMessage(QString("Ready"),2000);
    
	/** 2. Thumnail Scroll Area **/
    thumbScrollArea  = new QScrollArea(this);
    centralWidgetLayout->addWidget(thumbScrollArea);
    thumbScrollArea->setFixedHeight(THUMBNAILSIZE+32);
    thumbScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    Thumbnails = new thumbnailSeries(this,&origImages);
    thumbScrollArea->setWidget(Thumbnails);

    Thumbnails->show();
    BufferFile = new QFile (currentDirectory + "\\ResultBuffer.tmp");
    if (BufferFile->exists()) BufferFile->remove();
    
    
    /** Set up result window **/
    resultsDialog = new QDialog (NULL,Qt::Window);
    QGridLayout* resultLayout = new QGridLayout();
    resultsDialog->setLayout(resultLayout);
    QHBoxLayout* resultsToolbarLayout = new QHBoxLayout;
    compImgWidget = new imageWidget (resultsDialog, 480, 320, &compositeImage);
    deptImgWidget = new imageWidget (resultsDialog, 480, 320, &depthImage);
    gradientThreshold = DEFAULT_GRADIENTTHRESHOLD;
    
    QPushButton* saveCompImageButton = new QPushButton (
                QIcon(":/icon/save.png"),QString(),compImgWidget);
    saveCompImageButton->setToolTip("Save composite image");
    
    QPushButton* saveDeptImageButton = new QPushButton (
                QIcon(":/icon/save.png"),QString(),deptImgWidget);
    saveDeptImageButton->setToolTip("Save depth image");
    
    QGroupBox*   selectImageSequence = new QGroupBox(QString ("Image order"),resultsDialog);
    QHBoxLayout* selectImageSeqLayout = new QHBoxLayout;
    QRadioButton* selectTopDownButton = new QRadioButton("Top-down",selectImageSequence);
    QRadioButton* selectBottomUpButton = new QRadioButton("Bottom-up",selectImageSequence);
    
    selectImageSequence->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    selectImageSequence->setLayout(selectImageSeqLayout);    
    selectImageSeqLayout->addWidget(selectTopDownButton);
    selectImageSeqLayout->addWidget(selectBottomUpButton);
    
    QPushButton* setThreshold = new QPushButton(QIcon(":/icon/setThreshold.png"),QString(),resultsDialog);
    setThreshold->setToolTip("Set the threshold for non-variant pixels");
    
    resultsToolbarLayout->addWidget(selectImageSequence,0,Qt::AlignLeft);
    resultsToolbarLayout->addWidget(setThreshold,1,Qt::AlignLeft);

    selectTopDownButton-> setToolTip(QString("Images from TOP surface to BOTTOM"));
    selectBottomUpButton->setToolTip(QString("Images from BOTTOM to TOP surface"));
    selectTopDownButton->setChecked(1);
    fromTopToBottom=1;

    resultLayout->addLayout(resultsToolbarLayout,0,0,1,-1);
    resultLayout->addWidget(compImgWidget,1,0,1,1);
    resultLayout->addWidget(deptImgWidget,1,1,1,1);

    QGridLayout* compImageWidgetLayout = new QGridLayout;
    compImgWidget->setLayout(compImageWidgetLayout);
    compImageWidgetLayout->addWidget(saveCompImageButton,0,0,1,1,Qt::AlignLeft|Qt::AlignBottom);
    
    QGridLayout* deptImageWidgetLayout = new QGridLayout;
    deptImgWidget->setLayout(deptImageWidgetLayout);
    deptImageWidgetLayout->addWidget(saveDeptImageButton);
    deptImageWidgetLayout->setAlignment(saveDeptImageButton,Qt::AlignLeft|Qt::AlignBottom);

    /** Connect signal slots **/    
    connect (actionLoadImage, &QAction::triggered, this, &loadImageSet);
    connect (actionSetMag, &QAction::triggered, this, &setMag);
    connect (actionSetRad, &QAction::triggered, this, &setRad);
    connect (actionViewResults, &QAction::triggered, resultsDialog, &QDialog::show);
    connect (actionAnalyze, &QAction::triggered, this, &analyzeImages);
    
    connect(Thumbnails,&thumbnailSeries::activeImageChanged, 
            this, &MainWindow::activeImageUpdated);
    
    connect (selectTopDownButton, SIGNAL(clicked(bool)), this, SLOT(selectTopDown()));
    connect (selectBottomUpButton, SIGNAL(clicked(bool)), this, SLOT(selectBottomUp()));
    connect (saveCompImageButton,&QPushButton::clicked, this, &saveCompImage);
    connect (saveDeptImageButton,&QPushButton::clicked, this, &saveDeptImage);
    connect (setThreshold,&QPushButton::clicked, this, &setGradientThreshold);

    resultsDialog->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

MainWindow::~MainWindow() {
}

void MainWindow::updateResults()
{

    if (!imagesAnalyzed) return;
    /** Generate depth image **/
    
    compositeImage = QImage (imageW, imageH, QImage::Format_RGB32);
    compositeImage.fill(Qt::black);
    depthImage = QImage (imageW, imageH, QImage::Format_RGB32);
    depthImage.fill(Qt::white);

    const unsigned char bottomHue = 240;
    const unsigned char topHue = 0;
    int Margin = Analyzer->getMargin();
    int currentImageNo = 0;
    float totalLayers = origImages.size() - 1; //So that layers go from 0:totalLayers
    int actualGradThreshold = (gradientThreshold/100.0)*(gradientThreshold/100.0)*255*255
            *Analyzer->getSpotSize()*3;
    
    for (QStringList::iterator iFile = loadFileNames.begin();
         iFile!= loadFileNames.end(); iFile++, currentImageNo++){
        QImage currentImage(*iFile);
        for (int iY = Margin; iY<imageH-Margin; iY++){
            for (int iX = Margin; iX<imageW-Margin; iX++){
                int zDepth = zMatrix[iY-Margin][iX-Margin];
                if (currentImageNo == zDepth){
                    compositeImage.setPixel(iX, iY, currentImage.pixel(iX,iY));
                }
                
                float fracDepthFromBottom;
                
                if (maxGradient[iY-Margin][iX-Margin]>actualGradThreshold) {
                    fracDepthFromBottom = zMatrix[iY-Margin][iX-Margin] / totalLayers;
                    if (fromTopToBottom) fracDepthFromBottom = 1-fracDepthFromBottom;
                }
                else {
                    fracDepthFromBottom = 0;
                }
                
                depthImage.setPixelColor(iX, iY, QColor::fromHsv(fracDepthFromBottom*
                                                    (topHue-bottomHue)+bottomHue,255,255));
            }
        }
    }
    
    compImgWidget->update();
    deptImgWidget->update();
}

void MainWindow::loadImageSet(){
    /** First, get the file names **/
    
    QStringList getLoadFileNames = QFileDialog::getOpenFileNames(this,
                QString ("Open"),currentDirectory,
                QString ("Images (*.jpg *.png *.bmp *.tif *.tiff)"));
    if (getLoadFileNames.isEmpty()) return; 
    loadFileNames = getLoadFileNames;//At least 1 image is valid then proceed
    
    /* Extract the current directory*/
    int LastBackSlash = loadFileNames.back().lastIndexOf("\\");
    if (LastBackSlash!= -1){
        currentDirectory = loadFileNames.back();
        currentDirectory.remove(LastBackSlash,-1);
    }
    
    imagesAnalyzed = 0;
    imageW = imageH = 0;
    loadFileNames.sort();
    
    /** Then, load the files into memory **/
    QStringList::iterator iFile = loadFileNames.begin();
    bool dimensionSet = 0; //Marks true once a image is loaded.
    
    int imagesCount = 0;
    origImages.clear();
    while (iFile!= loadFileNames.end()){
        QImage currentImage(*iFile);
        if (currentImage.isNull()) {
            iFile = loadFileNames.erase(iFile); // If fails to load,then remove file name
            continue;
        }
        
        if (!dimensionSet) { //Set dimensions if not already done 
            dimensionSet = 1;
            imageW = currentImage.width();
            imageH = currentImage.height();
        }
        else { //Check consistency of image size
            if (currentImage.width()!=imageW || currentImage.height()!=imageH){
                iFile = loadFileNames.erase(iFile); // Image dimensions mismatch
                QMessageBox::warning (this,QString ("Error"), QString ("Image dimensions mismatch"));
                continue;
            }
        }
        
        origImages.push_back(currentImage.scaled(400,400,Qt::KeepAspectRatio)); //Generate thumbnail

        Thumbnails->updateThumbnails();
        
        currentDisplayedImage = imagesCount++;
        origImgWidget->setImage(&(origImages[currentDisplayedImage]));

        iFile++;
    }
    if (BufferFile->exists()) BufferFile->remove(); // Remove buffer if existing
    update();
}

void MainWindow::analyzeImages()
{
    if (!origImages.size()) return;
    QMessageBox::StandardButton promptResponse = QMessageBox::question(this, "Warning",
        QString("Depending on the image size and number, analysis may take several minutes.\n")+
        QString("During this time, the program may be temporarily unresponsive.\n")+
        QString("Do you wish to continue?"));
    
    if (promptResponse==QMessageBox::No || promptResponse==QMessageBox::Cancel) return;
                                                                       
    imagesAnalyzed = 0;
    if (BufferFile->exists()) BufferFile->remove();
    
    int Margin = Analyzer->getMargin();
    zMatrix = QVector <QVector <int> >(imageH-2*Margin, QVector <int>(imageW-2*Margin,-1));
    maxGradient = QVector <QVector <int32_t> >
            (imageH-2*Margin, QVector <int32_t>(imageW-2*Margin,0));
    
    int currentImageNo=0;
    for (QStringList::iterator iFile = loadFileNames.begin();
         iFile!= loadFileNames.end(); iFile++, currentImageNo++){
        QImage currentImage;
        currentImage.load(*iFile);
        Analyzer->setImage(&currentImage);
        Thumbnails->setActiveImage(currentImageNo);
        Thumbnails->update();
        origImgWidget->update();
        origImgWidget->setImage(&origImages[currentImageNo]);
        statusBar()->showMessage(QString("Analyzing ")+QString::number(currentImageNo+1),10000);
        int timeSpent = Analyzer->analyzeGrad();
        if (timeSpent==-1) return;
        
        BufferFile->open(QFile::WriteOnly|QFile::Append);
        for (int iY=Margin; iY<imageH-Margin; iY++){

            BufferFile->write(
                (char*) (&(Analyzer->getResult()[iY-Margin][0])),
                (imageW-2*Margin)*sizeof (int32_t));
            
            //Compare image sharpness
            for (int iX=Margin; iX<imageW-Margin; iX++){
                if (maxGradient[iY-Margin][iX-Margin]<Analyzer->getResult()[iY-Margin][iX-Margin]){
                    maxGradient[iY-Margin][iX-Margin]=Analyzer->getResult()[iY-Margin][iX-Margin];
                    zMatrix[iY-Margin][iX-Margin] = currentImageNo;
                }
            }
        }
        gradImgWidget->update();
        BufferFile->close(); //conclude writing
    }
    imagesAnalyzed = 1;
    
    updateResults();

    resultsDialog->show();
}

void MainWindow::setRad(){
    int Response = QMessageBox::question(
                this,QString ("Confirm"),
                QString ("Changing radius will evaluate the series. Confirm?"),
                QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel));
    
    switch (Response){
    case QMessageBox::No: case QMessageBox::Cancel:
        return;
    case QMessageBox::Yes:
        bool NumberOK;
        float newValue = QInputDialog::getDouble(this,
            "Input radius", "Input radius in pixels", 
            Analyzer->getRadius(),0,1024,1, &NumberOK);
        if (NumberOK) {
            Analyzer->setRadius(newValue);
        }
        gradImgWidget->update();    
    }
    imagesAnalyzed = 0;
    activeImageUpdated(currentDisplayedImage);
}

void MainWindow::setMag(){
    bool NumberOK;
    float newValue = QInputDialog::getDouble(this,
        "Input contrast", "Input contrast factor (0~255)", 
        contrastMagnification,0,255,1, &NumberOK);
    if (NumberOK) {
        contrastMagnification = newValue;
    }    
    activeImageUpdated(currentDisplayedImage);
}

void MainWindow::setGradientThreshold()
{
    bool NumberOK;
    float newValue = QInputDialog::getDouble(resultsDialog,
        "Input threshold", "Input gradient threshold (0~100)", 
        gradientThreshold,0,100,1, &NumberOK);
    if (NumberOK) {
        gradientThreshold = newValue;
    }
    updateResults();
    deptImgWidget->update();
}

void MainWindow::activeImageUpdated(int iImg)
{
    
    if (iImg <0 || iImg>origImages.size()) return;
    
    currentDisplayedImage = iImg;
    origImgWidget->setImage(&(origImages[iImg]));
    origImgWidget->update();
    
    if (!imagesAnalyzed) return;
    
    ///Render the gradient map
    int Margin = Analyzer->getMargin();
    int sizeOfGradImg=(imageW-2*Margin)*(imageH-2*Margin)*sizeof(int32_t);
    if (BufferFile->size()<(iImg*sizeOfGradImg)) return;
    gradImage = QImage(imageW, imageH, QImage::Format_RGB32);
    gradImage.fill(Qt::white);

    //Read buffer data
    char* lineFromBuffer = new char [sizeOfGradImg];
    BufferFile->open(QIODevice::ReadOnly);
    BufferFile->seek(iImg*sizeOfGradImg);
    BufferFile->read(lineFromBuffer,sizeOfGradImg);
    BufferFile->close();
    
    int* gradData = (int*) lineFromBuffer;
    
    for (int iY = Margin; iY<imageH-Margin; iY++){
        for (int iX = Margin; iX<imageW-Margin; iX++){
            float gradValue = sqrt(
                    gradData[(iY-Margin)*(imageW-2*Margin)+iX-Margin]/
                    (3.0*Analyzer->getSpotSize()))*contrastMagnification;
            if (gradValue>255) gradValue = 255;
            gradImage.setPixelColor(iX, iY, QColor::fromRgb(255-gradValue,255-gradValue, 255));
        }
    }
    delete[] lineFromBuffer;
    gradImgWidget->update();
}

void MainWindow::aboutMe()
{
    QMessageBox::about(this,QString ("About 3DMicroscope"),
                       QString ("3DMicroscope v. ")+ Version +
                       QString ("\nWritten by Alex Qing Ni with Qt (TM)")+
                       QString ("\nEmail: mr.alchem@gmail.com")+
                       QString ("\n\nLicensed under GPL v3"));
}

void MainWindow::aboutQt(){
    QMessageBox::aboutQt(this,QString("About Qt"));
}

void MainWindow::selectTopDown() {
    fromTopToBottom=1;
    updateResults();
}

void MainWindow::selectBottomUp() {
    fromTopToBottom=0;
    updateResults();
}

void MainWindow::openResultsWindow()
{
    resultsDialog->show();
}

void MainWindow::saveCompImage()
{
    QString saveFileName = QFileDialog::getSaveFileName(resultsDialog,
                QString("Save"),currentDirectory,QString("JPEG (*.jpg)"));
    compositeImage.save(saveFileName,"JPG",95);
}

void MainWindow::saveDeptImage()
{
    QString saveFileName = QFileDialog::getSaveFileName(resultsDialog,
                QString("Save"),currentDirectory,QString("JPEG (*.jpg)"));
    depthImage.save(saveFileName,"JPG",95);
}
