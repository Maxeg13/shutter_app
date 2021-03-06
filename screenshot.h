

#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QPixmap>
#include <QWidget>

class QCheckBox;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QSpinBox;
class QVBoxLayout;

class Screenshot : public QWidget
{
    Q_OBJECT

public:
    Screenshot();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void oneSend();
    void newScreenshot();
    void saveScreenshot();
    void shootScreen();
    void COMInit();
    void updateCheckBox();

private:
    void updateScreenshotLabel();

    QPixmap originalPixmap;

    QLabel *screenshotLabel;
    QSpinBox *sizeSpinBox;
    QCheckBox *hideThisWindowCheckBox;
    QPushButton *newScreenshotButton;
};

#endif // SCREENSHOT_H
