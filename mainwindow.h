#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <cstring>
#include <QMainWindow>
#include <vector>
#include <QDebug>
#include <QImage>
#include <stdlib.h>
#include <QPainter>
#include <cmath>
#include <QTimer>
#include <iostream>
#include <QString>
#include <utility>
#include <QColor>

#include "vector2d.h"
#include "plasmaball.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  bool isRunning;
  int distCompSteps;
  unsigned int imgFrameCounter; // counts how many frames are exported
  PlasmaBall * plasmaBall;

  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow( );

private:
  Ui::MainWindow *ui;

public slots:
  void Next( );
  void UpdatePaths( );


private slots:
  void on_runButton_clicked();
  void on_updateDistButton_clicked();
  void on_distPowBox_valueChanged(double arg1);
  void on_stopValBox_valueChanged(double arg1);
  void on_saveButton_clicked();
};

#endif // MAINWINDOW_H
