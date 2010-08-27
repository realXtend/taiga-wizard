#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>
#include <QtGui/QVBoxLayout>
#include <QMessageBox>
#include <QMap>
#include "confighandler.h"
#include "regionwizard.h"
#include "WizardInterface.h"


namespace Ui {
    class Wizard;
}


class Wizard : public QWizard, public WizardInterface {
    Q_OBJECT

public:
    Wizard(QWidget *parent = 0, RegionWizard* regionWizard = 0);
    ~Wizard();

    void SetTaigaPath(QString path);

    RegionWizard* m_RegionWizard;
    QMessageBox* regionConflict;


    void Configure(bool foundTaiga);
    bool m_GridConfig;
    bool m_JoinGrid;
    bool m_Defaults;
    bool m_Regions;


    void ClearRegions();

    virtual QList<int> GetRegionPorts();
    void SetDataBaseParams();

protected:
    void changeEvent(QEvent *e);
    bool checkTaigaDir();
    void configureSimPage();
    void configureGridPage();
    void configureDbPage();
    void configureRegionPage();
    void setRegionDefaults();
    void constructDatabaseConnectionStringAndDBPlugin();
    void setComboDefault(QComboBox* combo, QString plugin);
    void loadDefaults();
    void setRegionsFromIni();
    void setDatabasesFromConfig();
    void setWritableFiles();

    QString getCurrentDbName(QMap<QString, QMap<QString,QString>* >& configIniSections, QString sectionName, QString fileName, QString searchString);
    void setCurrentDbName(QMap<QString, QMap<QString,QString>* >& configIniSections, QString sectionName, QString fileName, QString searchString, QString newName);


    QMessageBox* m_missingUserName;
    QMessageBox* m_regionNotification;
    QMessageBox* m_regionNotSelected;


private:
    Ui::Wizard *ui;
    QVBoxLayout m_VBoxLaout;
    QSpacerItem *m_Spacer;

    ConfigHandler m_configHandler;


    int m_lastId;

private slots:
    void on_comboDBPlugin_currentIndexChanged(QString );
    void on_lineEditSaveName_textChanged(QString );
    void on_lineEditGridDBPassword_textChanged(QString );
    void on_btnChangeDataBaseNames_clicked();
    void on_btnRemoveRegion_clicked();
    void on_btnSaveConfigs_clicked();
    void on_lineEditSimGridUrl_textChanged(QString );
    void on_checkBoxRobust_clicked();
    void on_btnBrowseDirectory_clicked();
    void on_btnUpdateRegion_clicked();
    void on_btnAddRegion_clicked();
    void onNext_button_clicked();
    void onFinish_button_clicked();
};

#endif // WIZARD_H
