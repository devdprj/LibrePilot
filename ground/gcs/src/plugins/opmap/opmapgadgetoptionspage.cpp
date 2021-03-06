/**
 ******************************************************************************
 *
 * @file       opmapgadgetoptionspage.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2012.
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup OPMapPlugin OpenPilot Map Plugin
 * @{
 * @brief The OpenPilot Map plugin
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "opmapgadgetoptionspage.h"
#include "opmapgadgetconfiguration.h"
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

#include "opmapcontrol/opmapcontrol.h"
#include "utils/pathutils.h"
#include "ui_opmapgadgetoptionspage.h"

// *********************************************

OPMapGadgetOptionsPage::OPMapGadgetOptionsPage(OPMapGadgetConfiguration *config, QObject *parent) :
    IOptionsPage(parent),
    m_config(config)
{}

QWidget *OPMapGadgetOptionsPage::createPage(QWidget *parent)
{
    int index;

    m_page = new Ui::OPMapGadgetOptionsPage();
    QWidget *w = new QWidget(parent);
    m_page->setupUi(w);

    // populate the map provider combobox
    m_page->providerComboBox->clear();
    m_page->providerComboBox->addItems(mapcontrol::Helper::MapTypes());

    // populate the access mode combobox
    m_page->accessModeComboBox->clear();
    m_page->accessModeComboBox->addItems(mapcontrol::Helper::AccessModeTypes());

    index = m_page->providerComboBox->findText(m_config->mapProvider());
    index = (index >= 0) ? index : 0;
    m_page->providerComboBox->setCurrentIndex(index);

    // populate the map max update rate combobox
    m_page->maxUpdateRateComboBox->clear();
    m_page->maxUpdateRateComboBox->addItem("100ms", 100);
    m_page->maxUpdateRateComboBox->addItem("200ms", 200);
    m_page->maxUpdateRateComboBox->addItem("500ms", 500);
    m_page->maxUpdateRateComboBox->addItem("1 sec", 1000);
    m_page->maxUpdateRateComboBox->addItem("2 sec", 2000);
    m_page->maxUpdateRateComboBox->addItem("5 sec", 5000);

    index = m_page->maxUpdateRateComboBox->findData(m_config->maxUpdateRate());
    index = (index >= 0) ? index : 4;
    m_page->maxUpdateRateComboBox->setCurrentIndex(index);

    // populate the safety area radius combobox
    m_page->safeAreaRadiusComboBox->clear();
    m_page->safeAreaRadiusComboBox->addItem("5m", 5);
    m_page->safeAreaRadiusComboBox->addItem("10m", 10);
    m_page->safeAreaRadiusComboBox->addItem("20m", 20);
    m_page->safeAreaRadiusComboBox->addItem("50m", 50);
    m_page->safeAreaRadiusComboBox->addItem("100m", 100);
    m_page->safeAreaRadiusComboBox->addItem("200m", 200);
    m_page->safeAreaRadiusComboBox->addItem("500m", 500);
    m_page->safeAreaRadiusComboBox->addItem("1000m", 1000);
    m_page->safeAreaRadiusComboBox->addItem("2000m", 2000);
    m_page->safeAreaRadiusComboBox->addItem("5000m", 5000);

    index = m_page->safeAreaRadiusComboBox->findData(m_config->safeAreaRadius());
    index = (index >= 0) ? index : 0;
    m_page->safeAreaRadiusComboBox->setCurrentIndex(index);

    m_page->checkBoxShowSafeArea->setChecked(m_config->showSafeArea());

    m_page->zoomSpinBox->setValue(m_config->zoom());
    m_page->latitudeSpinBox->setValue(m_config->latitude());
    m_page->longitudeSpinBox->setValue(m_config->longitude());

    m_page->checkBoxUseOpenGL->setChecked(m_config->useOpenGL());
    m_page->checkBoxShowTileGridLines->setChecked(m_config->showTileGridLines());

    index = m_page->accessModeComboBox->findText(m_config->accessMode());
    index = (index >= 0) ? index : 0;
    m_page->accessModeComboBox->setCurrentIndex(index);

    m_page->checkBoxUseMemoryCache->setChecked(m_config->useMemoryCache());

    m_page->lineEditCacheLocation->setExpectedKind(Utils::PathChooser::Directory);
    m_page->lineEditCacheLocation->setPromptDialogTitle(tr("Choose Cache Directory"));
    m_page->lineEditCacheLocation->setPath(m_config->cacheLocation());

    QDir dir(":/uavs/images/");
    QStringList list = dir.entryList();
    foreach(QString i, list) {
        QIcon icon(QPixmap(":/uavs/images/" + i));

        m_page->uavSymbolComboBox->addItem(icon, QString(), i);
    }
    for (int x = 0; x < m_page->uavSymbolComboBox->count(); ++x) {
        if (m_page->uavSymbolComboBox->itemData(x).toString() == m_config->uavSymbol()) {
            m_page->uavSymbolComboBox->setCurrentIndex(x);
        }
    }

    m_page->defaultWaypointAltitude->setValue(m_config->defaultWaypointAltitude());
    m_page->defaultWaypointVelocity->setValue(m_config->defaultWaypointVelocity());
    connect(m_page->pushButtonCacheDefaults, SIGNAL(clicked()), this, SLOT(on_pushButtonCacheDefaults_clicked()));

    return w;
}

void OPMapGadgetOptionsPage::on_pushButtonCacheDefaults_clicked()
{
    int index = m_page->accessModeComboBox->findText("ServerAndCache");

    index = (index >= 0) ? index : 0;
    m_page->accessModeComboBox->setCurrentIndex(index);

    m_page->checkBoxUseMemoryCache->setChecked(true);
    m_page->lineEditCacheLocation->setPath(Utils::GetStoragePath() + "mapscache" + QDir::separator());
}

void OPMapGadgetOptionsPage::apply()
{
    m_config->setMapProvider(m_page->providerComboBox->currentText());
    m_config->setZoom(m_page->zoomSpinBox->value());
    m_config->setLatitude(m_page->latitudeSpinBox->value());
    m_config->setLongitude(m_page->longitudeSpinBox->value());
    m_config->setUseOpenGL(m_page->checkBoxUseOpenGL->isChecked());
    m_config->setShowTileGridLines(m_page->checkBoxShowTileGridLines->isChecked());
    m_config->setAccessMode(m_page->accessModeComboBox->currentText());
    m_config->setUseMemoryCache(m_page->checkBoxUseMemoryCache->isChecked());
    m_config->setCacheLocation(m_page->lineEditCacheLocation->path());
    m_config->setUavSymbol(m_page->uavSymbolComboBox->itemData(m_page->uavSymbolComboBox->currentIndex()).toString());
    m_config->setMaxUpdateRate(m_page->maxUpdateRateComboBox->itemData(m_page->maxUpdateRateComboBox->currentIndex()).toInt());
    m_config->setSafeAreaRadius(m_page->safeAreaRadiusComboBox->itemData(m_page->safeAreaRadiusComboBox->currentIndex()).toInt());
    m_config->setShowSafeArea(m_page->checkBoxShowSafeArea->isChecked());
    m_config->setDefaultWaypointAltitude(m_page->defaultWaypointAltitude->value());
    m_config->setDefaultWaypointVelocity(m_page->defaultWaypointVelocity->value());
}

void OPMapGadgetOptionsPage::finish()
{
    delete m_page;
}
