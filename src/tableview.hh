#ifndef TABLEVIEW_HH
#define TABLEVIEW_HH

#include <QTableView>
#include "receiver.hh"

/** Displays the received spots. */
class TableView : public QTableView
{
  Q_OBJECT
public:
  explicit TableView(Receiver &rx, QWidget *parent = 0);

protected:
  Receiver &_rx;
};

#endif // TABLEVIEW_HH
