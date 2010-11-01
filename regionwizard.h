#ifndef REGIONWIZARD_H
#define REGIONWIZARD_H

#include <QWizard>
#include <QFormLayout>

#include <WizardInterface.h> // because of circular dependency


namespace Ui {
    class RegionWizard;
}

class Wizard;

typedef QList<int> (*getRegionPortsMethod)();

class RegionWizard : public QWizard {
    Q_OBJECT
public:
    RegionWizard(QWidget *parent = 0);
    ~RegionWizard();
    QGridLayout* GetRegionGridLayout();
    QFormLayout* GetAvatarLayout();
    void GoToStart();
    void SetDefaults();
    WizardInterface *m_Wizard;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::RegionWizard *ui;

private slots:
    void on_btnNextPort_clicked();
    void on_btnNewUUID_clicked();
};

#endif // REGIONWIZARD_H
