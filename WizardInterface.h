#ifndef WIZARDINTERFACE_H
#define WIZARDINTERFACE_H

#include <QList>


class WizardInterface {
public:
   virtual QList<int> GetRegionPorts() = 0;
};


#endif // WIZARDINTERFACE_H
