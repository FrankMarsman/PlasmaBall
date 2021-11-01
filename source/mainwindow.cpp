#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  this->plasmaBall = new PlasmaBall(2000);

  isRunning = false;
  distCompSteps = 0;
  this->imgFrameCounter = 1;

  on_runButton_clicked( );

  QTimer::singleShot(40, this, SLOT(Next( )));
  //QTimer::singleShot(100, this, SLOT(UpdatePaths( )));
  //QTimer::singleShot(3000, this, SLOT(on_runButton_clicked( )));
} // MainWindow

MainWindow::~MainWindow( ) {
  delete ui;
} // MainWindow

void MainWindow::Next( ) {
  bool drawPaths = ui->drawPath->isChecked( );
  bool drawDist = ui->drawDist->isChecked( );

  plasmaBall->r_draw = 0.1 * double(ui->rSlider->value( ));
  plasmaBall->w_draw = 0.1 * double(ui->wSlider->value( ));
  plasmaBall->Nfix = ui->nCores->value( );
  plasmaBall->coreSize = ui->rCoreBox->value( );

  if (isRunning == false) {
    QImage img = plasmaBall->ToQImage(drawPaths, drawDist);

    ui->imgLabel->setPixmap(QPixmap::fromImage(img));

    QTimer::singleShot(1000, this, SLOT(Next( )));
    return;
  } // if

  time_t t = clock( );
  int N = ui->nSpin->value( );

  for (int i = 0; i < N; i++)
    this->plasmaBall->Next(0.01);

  if (ui->distCheck->isChecked( ) && ((plasmaBall->stepNr / N) % ui->nBox->value( ) == 0))
    on_distPowBox_valueChanged(ui->distPowBox->value( ));

  distCompSteps = 0;


  QImage img = plasmaBall->ToQImage(drawPaths, drawDist);
  ui->imgLabel->setPixmap(QPixmap::fromImage(img));

  if (ui->animCheck->isChecked( )) { // save frame
    QString frameStr = QString::number(this->imgFrameCounter);
    while (frameStr.length( ) < 3)
      frameStr = "0" + frameStr;

    QString str = "/Users/fmarsman/Dropbox/Qt_Projects/PlasmaSim1/zframe_";
    str += frameStr;
    str += ".png";
    auto active_window = qApp->activeWindow();
    if (active_window) { // could be null if your app doesn't have focus
      QPixmap pixmap(active_window->size());
      active_window->render(&pixmap);
      QImage image = pixmap.toImage();
      bool success = image.save(str, "png");
      qDebug( ) << "SImage size!" << image.width( ) << "sucess = " << success << "name = " << str;
      this->imgFrameCounter++;
    } // if
  } // if

  double dt = double(clock( ) - t) / CLOCKS_PER_SEC;
  double freq = double(N) / dt;
  freq /= 1000;

  ui->statusBar->showMessage("#Steps = " + QString::number(plasmaBall->stepNr/1000) + "k. "
                             + "Step speed = " + QString::number(freq) + " kHz.");

  QTimer::singleShot(1, this, SLOT(Next( )));
} // Next


void MainWindow::UpdatePaths( ) {
  if (isRunning == false) {
    QTimer::singleShot(100, this, SLOT(UpdatePaths( )));
    return;
  } // if

  time_t t = clock( );
  if (plasmaBall->distPower != ui->distPowBox->value( )) {
    plasmaBall->distPower = ui->distPowBox->value( );
    plasmaBall->FillnBdistVec( );
  } // if

  distCompSteps = 0;
  plasmaBall->UpdateDistToEdge(true);

  double dt = 0;
  double tMax = 100;
  while (dt < tMax) {
    distCompSteps++;
    plasmaBall->UpdateDistToEdge(false);
    dt = double(clock( ) - t) / CLOCKS_PER_SEC;
    dt *= 1000;
  } // while
  QTimer::singleShot(500, this, SLOT(UpdatePaths( )));
} // UpdatePaths


void MainWindow::on_runButton_clicked( ) {
  if (isRunning) {
    isRunning = false;
    ui->runButton->setText("Run");
  } // if
  else {
    isRunning = true;
    ui->runButton->setText("Pause");
  } // else
} // on_runButton_clicked


void MainWindow::on_updateDistButton_clicked( ) {
  time_t t = clock( );
  bool done;
  if (distCompSteps == 0) {
    done = plasmaBall->UpdateDistToEdge(true);
  } // if
  else {
    done = plasmaBall->UpdateDistToEdge(false);
  } // else

  distCompSteps++;

  double dt = double(clock( ) - t) / CLOCKS_PER_SEC;
  dt *= 1000;

  ui->statusBar->showMessage("Done with dists = " + QString::number(done)
                             + ". time to compute = " + QString::number(dt) + "ms."
                             + " #compSteps = " + QString::number(distCompSteps));
} // on_updateDistButton_clicked


void MainWindow::on_distPowBox_valueChanged(double arg1) {
  //qDebug( ) << "on_distPowBox_valueChanged:" << arg1;

  time_t t = clock( );

  plasmaBall->distPower = arg1;
  plasmaBall->FillnBdistVec( );

  distCompSteps = 0;


  plasmaBall->UpdateDistToEdge(true);

  double dt1 = double(clock( ) - t) / CLOCKS_PER_SEC;
  dt1 *= 1000;
  t = clock( );

  double stopThreshold = ui->stopValBox->value( ) / 100;
  uint minUpdates = plasmaBall->N * stopThreshold;

  while (plasmaBall->UpdateDistToEdge(false) == false) {
    if (plasmaBall->updatedParticles.size( ) < minUpdates)
      break;
    if (distCompSteps > 1)
      break;
    //qDebug( ) << "   --- distCompSteps =" << distCompSteps;
    distCompSteps++;
  } // while

  double dt2 = double(clock( ) - t) / CLOCKS_PER_SEC;
  dt2 *= 1000;

  //ui->statusBar->showMessage("T first part = " + QString::number(dt1) + "ms."
  //                           + " T rest = " + QString::number(dt2) + "ms."
  //                           + " #compSteps = " + QString::number(distCompSteps));
} // on_distPowBox_valueChanged


void MainWindow::on_stopValBox_valueChanged(double arg1) {
  on_distPowBox_valueChanged(ui->distPowBox->value( ));
} // on_stopValBox_valueChanged



// saves current image
void MainWindow::on_saveButton_clicked( ) {
  qDebug( ) << "Saving image!";
  bool drawPaths = ui->drawPath->isChecked( );
  bool drawDist = ui->drawDist->isChecked( );
  QString str = "/Users/fmarsman/Dropbox/Qt_Projects/PlasmaSim1/AAA.png";
  QImage img = plasmaBall->ToQImage(drawPaths, drawDist);
  img.save(str, ".png");

} // on_saveButton_clicked





