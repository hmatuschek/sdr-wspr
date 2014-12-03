#include "tableview.hh"

TableView::TableView(Receiver &rx, QWidget *parent)
  : QTableView(parent), _rx(rx)
{
  setModel(_rx.messages());
}
