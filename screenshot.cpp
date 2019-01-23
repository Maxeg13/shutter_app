//ask about the port baudrate
#include <QtWidgets>
#include <QtSerialPort>
#include "screenshot.h"

QTimer timer;
QSerialPort port;
QImage imgh;
QLineEdit* serial_le;
QLineEdit* command_le;

bool serial_inited;

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
    serial_le=new QLineEdit("COM3");
    serial_le->setMaximumWidth(100);

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

    QLabel* label_com=new QLabel("port name");

    QGridLayout *optionsGroupBoxLayout = new QGridLayout(optionsGroupBox);
    optionsGroupBoxLayout->addWidget(new QLabel(tr("size:"), this), 0, 0);
    optionsGroupBoxLayout->addWidget(sizeSpinBox, 0, 1);
    optionsGroupBoxLayout->addWidget(label_com, 1, 0);
    optionsGroupBoxLayout->addWidget(serial_le, 1, 1);

    connect(serial_le,SIGNAL(returnPressed()),this,SLOT(COMInit()));

//    optionsGroupBoxLayout->addWidget(hideThisWindowCheckBox, 1, 0, 1, 2);

    mainLayout->addWidget(optionsGroupBox);

    QHBoxLayout *commandsLayout = new QHBoxLayout;
    command_le=new QLineEdit("asssb");
    commandsLayout->addWidget(command_le);
    mainLayout->addLayout(commandsLayout);

    connect(command_le, SIGNAL(returnPressed()),this,SLOT(oneSend()));
//    QHBoxLayout *buttonsLayout = new QHBoxLayout;
//    newScreenshotButton = new QPushButton(tr("New Screenshot"), this);
//    connect(newScreenshotButton, &QPushButton::clicked, this, &Screenshot::newScreenshot);
//    buttonsLayout->addWidget(newScreenshotButton);
//    QPushButton *saveScreenshotButton = new QPushButton(tr("Save Screenshot"), this);
//    connect(saveScreenshotButton, &QPushButton::clicked, this, &Screenshot::saveScreenshot);
//    buttonsLayout->addWidget(saveScreenshotButton);
//    QPushButton *quitScreenshotButton = new QPushButton(tr("Quit"), this);
//    quitScreenshotButton->setShortcut(Qt::CTRL + Qt::Key_Q);
//    connect(quitScreenshotButton, &QPushButton::clicked, this, &QWidget::close);
//    buttonsLayout->addWidget(quitScreenshotButton);
//    buttonsLayout->addStretch();
//    mainLayout->addLayout(buttonsLayout);

    shootScreen();
    sizeSpinBox->setValue(10);

    setWindowTitle(tr("SHUTTER APP"));
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

void Screenshot::oneSend()
{
    QByteArray ba;
    QString str=serial_le->text();
    ba=str.toUtf8();
    ba.push_back(25);
    ba.push_back('a');

    port.write(ba);
}

void Screenshot::COMInit()
{
    serial_inited=1;
    QString qstr=serial_le->text();
    std::string str1=qstr.toUtf8().constData();
    std::wstring str(str1.begin(),str1.end());
    //    hSerial=new Serial;
    //    port=new QSerialPort;
    port.setPortName(qstr);
    port.setBaudRate(38400);
//    if(port.open(QIODevice::WriteOnly))
//    {
//        QString message = tr("com port is successfully opened");
//        statusBar()->showMessage(message);
//    }
//    else
//    {
//        QString message = tr("com port is not opened");
//        statusBar()->showMessage(message);
//    }


    //    hSerial->InitCOM(str.c_str());//was L"COM5"
    serial_le->setDisabled(true);
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

//    newScreenshotButton->setDisabled(false);
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
