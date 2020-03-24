#include "clickablewidget.h"
ClickableWidget::ClickableWidget(QFrame *parent):QCustomPlot(parent){};
ClickableWidget::~ClickableWidget(){};
void ClickableWidget::mousePressEvent(QMouseEvent *event){emit clicked();};

