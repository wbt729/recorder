#include "meanplotwidget.h"

MeanPlotWidget::MeanPlotWidget(QWidget *parent) {
	sampler = new Sampler();
	setLayout(new QGridLayout);
	plot = new MeanPlot();
	hpSpinBox = new QSpinBox();
	lpSpinBox = new QSpinBox();
	hpSpinBox->setRange(0, 100);
	lpSpinBox->setRange(0, 100);
	hpSpinBox->setValue(30);
	lpSpinBox->setValue(3);

	sampler->setHP(hpSpinBox->value());
	sampler->setLP(lpSpinBox->value());

	layout()->addWidget(plot);
	layout()->addWidget(new QLabel("Highpass"));
	layout()->addWidget(hpSpinBox);
	layout()->addWidget(new QLabel("Lowpass"));
	layout()->addWidget(lpSpinBox);
	setMinimumSize(800, 600);
	connect(sampler, SIGNAL(newSamples(double, double, double)), plot, SLOT(updateData(double, double, double)));
	connect(hpSpinBox, SIGNAL(valueChanged(int)), sampler, SLOT(setHP(int)));
	connect(lpSpinBox, SIGNAL(valueChanged(int)), sampler, SLOT(setLP(int)));
}

MeanPlotWidget::~MeanPlotWidget() {

}

void MeanPlotWidget::newMat(cv::Mat *mat) {
	//qDebug() << "MeanPlotWidget: new mat";
	sampler->setMat(mat);
}

void MeanPlotWidget::newRoi(QRect roi) {
	//qDebug() << "MeanPlotWidget: new roi";
	sampler->setRoi(roi);
}

void MeanPlotWidget::closeEvent(QCloseEvent *event) {
	emit closing();
}