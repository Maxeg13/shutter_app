
#include <QtWidgets>

#include "screenshot.h"

QTimer timer;
QImage imgh;

int intens(QImage& img)
{
    float sum=0;
    for(int i=0;i<img.width();i++)
        for(int j=0;j<img.width();j++)
    sum+=qGray(img.pixel(i,j));

    return sum;
}

Screenshot::Screenshot()
    :  screenshotLabel(new QLabel(this))
{
    screenshotLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    screenshotLabel->setAlignment(Qt::AlignCenter);

    const QRect screenGeometry = QApplication::desktop()->screenGeometry(this);
    screenshotLabel->setMinimumSize(screenGeometry.width() / 8, screenGeometry.height() / 8);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(screenshotLabel);

    QGroupBox *optionsGroupBox = new QGroupBox(tr("Options"), this);
    sizeSpinBox = new QSpinBox(optionsGroupBox);
    sizeSpinBox->setSuffix(tr(" pixels"));
    sizeSpinBox->setMaximum(60);

    connect(sizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &Screenshot::updateCheckBox);

//    hideThisWindowCheckBox = new QCheckBox(tr("Hide This Window"), optionsGroupBox);

    QGridLayout *optionsGroupBoxLayout = new QGridLayout(optionsGroupBox);
    optionsGroupBoxLayout->addWidget(new QLabel(tr("size:"), this), 0, 0);
    optionsGroupBoxLayout->addWidget(sizeSpinBox, 0, 1);
//    optionsGroupBoxLayout->addWidget(hideThisWindowCheckBox, 1, 0, 1, 2);

    mainLayout->addWidget(optionsGroupBox);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    newScreenshotButton = new QPushButton(tr("New Screenshot"), this);
    connect(newScreenshotButton, &QPushButton::clicked, this, &Screenshot::newScreenshot);
    buttonsLayout->addWidget(newScreenshotButton);
    QPushButton *saveScreenshotButton = new QPushButton(tr("Save Screenshot"), this);
    connect(saveScreenshotButton, &QPushButton::clicked, this, &Screenshot::saveScreenshot);
    buttonsLayout->addWidget(saveScreenshotButton);
//    QPushButton *quitScreenshotButton = new QPushButton(tr("Quit"), this);
//    quitScreenshotButton->setShortcut(Qt::CTRL + Qt::Key_Q);
//    connect(quitScreenshotButton, &QPushButton::clicked, this, &QWidget::close);
//    buttonsLayout->addWidget(quitScreenshotButton);
    buttonsLayout->addStretch();
    mainLayout->addLayout(buttonsLayout);

    shootScreen();
    sizeSpinBox->setValue(10);

    setWindowTitle(tr("SHUTTER CONTROL"));
//    resize(200, 200);
    this->setMinimumSize(QSize(320,200));

//    QTimer::singleShot(sizeSpinBox->value() * 1000, this, &Screenshot::shootScreen);
    timer.start(35);
    connect(&timer,SIGNAL(timeout()),this,SLOT(shootScreen()));
}

void Screenshot::resizeEvent(QResizeEvent * /* event */)
{
    QSize scaledSize = originalPixmap.size();
    scaledSize.scale(screenshotLabel->size(), Qt::KeepAspectRatio);
    if (!screenshotLabel->pixmap() || scaledSize != screenshotLabel->pixmap()->size())
        updateScreenshotLabel();
}

void Screenshot::newScreenshot()
{
//    if (hideThisWindowCheckBox->isChecked())
//        hide();
//    newScreenshotButton->setDisabled(true);


}

void Screenshot::saveScreenshot()
{
    const QString format = "png";
    QString initialPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (initialPath.isEmpty())
        initialPath = QDir::currentPath();
    initialPath += tr("/untitled.") + format;

    QFileDialog fileDialog(this, tr("Save As"), initialPath);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDirectory(initialPath);
    QStringList mimeTypes;
    foreach (const QByteArray &bf, QImageWriter::supportedMimeTypes())
        mimeTypes.append(QLatin1String(bf));
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.selectMimeTypeFilter("image/" + format);
    fileDialog.setDefaultSuffix(format);
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString fileName = fileDialog.selectedFiles().first();
    if (!originalPixmap.save(fileName)) {
        QMessageBox::warning(this, tr("Save Error"), tr("The image could not be saved to \"%1\".")
                             .arg(QDir::toNativeSeparators(fileName)));
    }
}

void Screenshot::shootScreen()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();
    if (!screen)
        return;

//    if (sizeSpinBox->value() != 0)
//        QApplication::beep();

    originalPixmap = screen->grabWindow(0,this->x()-sizeSpinBox->value()+2,this->y()-sizeSpinBox->value()/2+16,sizeSpinBox->value(),sizeSpinBox->value());
    updateScreenshotLabel();

    QImage img=originalPixmap.toImage();
    img.convertToFormat(QImage::Format_Grayscale8);

    static float sumh;
    float sum=intens(img);
//    qDebug()<<sum;

    static int cnt;
    if(fabs(sumh-sum)>10)
    {
        cnt++;
        qDebug()<<cnt;
    }
    sumh=sum;

    newScreenshotButton->setDisabled(false);
//    if (hideThisWindowCheckBox->isChecked())
//        show();
}

void Screenshot::updateCheckBox()
{
//    if (sizeSpinBox->value() == 0) {
//        hideThisWindowCheckBox->setDisabled(true);
//        hideThisWindowCheckBox->setChecked(false);
//    } else {
//        hideThisWindowCheckBox->setDisabled(false);
//    }
}

void Screenshot::updateScreenshotLabel()
{
    screenshotLabel->setPixmap(originalPixmap.scaled(screenshotLabel->size(),
                                                     Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation));
}
