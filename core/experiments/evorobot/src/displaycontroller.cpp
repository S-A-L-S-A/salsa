// /********************************************************************************
//  *  SALSA Experiments Library                                                   *
//  *  Copyright (C) 2007-2012                                                     *
//  *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
//  *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
//  *  Gianluca Massera <emmegian@yahoo.it>                                        *
//  *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
//  *                                                                              *
//  *  This program is free software; you can redistribute it and/or modify        *
//  *  it under the terms of the GNU General Public License as published by        *
//  *  the Free Software Foundation; either version 2 of the License, or           *
//  *  (at your option) any later version.                                         *
//  *                                                                              *
//  *  This program is distributed in the hope that it will be useful,             *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
//  *  GNU General Public License for more details.                                *
//  *                                                                              *
//  *  You should have received a copy of the GNU General Public License           *
//  *  along with this program; if not, write to the Free Software                 *
//  *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
//  ********************************************************************************/
//
// #include "displaycontroller.h"
//
// #include <QtGui>        // Widget
// #include <QKeyEvent>    // Dialog
// #include <QFileDialog>  // Dialog
// #include <QMessageBox>  // Dialog
// #include <QImage>       // the background image of the controller
// #include <QComboBox>
// #include <QAction>
// #include "logger.h"
//
// // All the suff below is to avoid warnings on Windows about the use of unsafe
// // functions. This should be only a temporary workaround, the solution is stop
// // using C string and file functions...
// #if defined(_MSC_VER)
// 	#pragma warning(push)
// 	#pragma warning(disable:4996)
// #endif
//
// #define MAXFREEP 10000
//
// namespace salsa {
//
// NetworkDialog::NetworkDialog(Evonet* net, QWidget* parent, Qt::WindowFlags flags) : QWidget( parent, flags )
// {
// 	this->enet=net;
//
// 	cneuronn=0;              // n. of selected cneurons
// 	cscalex=1.0;             // x scale for the controller rendering
// 	cscaley=1.0;             // y scale for the controller rendering
// 	pseudomode=0;            // the display modality (0=label 1=delta, 2=weight, 3=biases, 4=gain)
//     enet->nselected=0;       // number of currently extracted parameters
// 	grid=0;                  // no grid
//
//     // Creating the main layout. Direction is BottomToTop so that m_layout (added first)
//     // is at bottom, while the toolbar (added last) is on top
//     m_mainLayout = new QVBoxLayout(this);
//     //m_mainLayout->setContentsMargins(0, 0, 0, 0);
//
//     createActions();
// 	// Creating toolbar and adding it to the main layout
// 	m_toolBar = new QToolBar(this);
// 	m_mainLayout->addWidget(m_toolBar);
// 	createToolBars();
//
// 	this->enet->neuronlesions = 0;
//
// 	freep=enet->freep;
//
// 	rendNetwork = new RendNetwork( this );
// 	m_mainLayout->addWidget( rendNetwork, 4 );
//
// 	mixerDialog = new MixerDialog( enet, this );
// 	m_mainLayout->addWidget( mixerDialog );
// 	mixerDialog->hide();
//
//     statusb = new QLabel("",this);
//     m_mainLayout->addWidget(statusb);
//
//     connect(rendNetwork, SIGNAL(selectedneuronsChanged()), this, SLOT(updatetoolb()) );
// }
//
//
// NetworkDialog::~NetworkDialog()
// {
// }
//
// /*
// void NetworkDialog::keyReleaseEvent(QKeyEvent* event)
// {
//
//     if (event->matches(QKeySequence::Print)) {
//         // Taking a screenshow of the widget
//         shotWidget();
//     } else {
//         // Calling parent function
//         QDialog::keyReleaseEvent(event);
//     }
// }
// */
//
// /*
// void NetworkDialog::shotWidget()
// {
//
//     // Taking a screenshot of this widget
//     QPixmap shot(size());
//     render(&shot);
//
//     // Asking the user where to save the shot
//     QString fileName = QFileDialog::getSaveFileName(this, tr("Save Shot"), "./widget.png", tr("Images (*.png *.xpm *.jpg)"));
//     if (!fileName.isEmpty()) {
//         shot.save(fileName);
//
//         QMessageBox::information(this, QString("File Saved"), QString("The widget shot has been saved"));
//     }
// }*/
//
// void NetworkDialog::setNet(Evonet* n)
// {
// 	// Here we replicate what is done in the constructor on the evonet passed there
// 	enet = n;
// 	enet->nselected=0;       // number of currently extracted parameters
// 	enet->neuronlesions = 0;
// 	freep=enet->freep;
// 	mixerDialog->setNet(enet);
// }
//
// void NetworkDialog::error(const char *emessage)
// {
//    QMessageBox::about(this, "ERROR", emessage);
// }
//
// void NetworkDialog::warning(const char *emessage)
// {
//    QMessageBox::about(this, "WARNING", emessage);
// }
//
//
// void NetworkDialog::createToolBars()
// {
//
// 	char stblock[1024];
// 	int b;
//
// 	m_toolBar->addAction(openAct);
// 	m_toolBar->addAction(saveAct);
//
// 	cblockAct = new QComboBox( m_toolBar );
// 	cblockAct->addItems(QStringList() << "blocks list");
// 	for (b=0; b < enet->net_nblocks; b++)
// 	{
// 		block_name(b, stblock);
// 		cblockAct->addItems(QStringList() << stblock);
// 	}
// 	connect( cblockAct, SIGNAL(currentIndexChanged(int)), this, SLOT(selblock_changed(int)) );
// 	connect(this, SIGNAL(selectedblockChanged()), this, SLOT(updateblocktype()) );
//
// 	m_toolBar->addWidget( cblockAct );
//
// 	blocktypeAct = new QComboBox( m_toolBar );
// 	blocktypeAct->addItems(QStringList() << "block type");
// 	blocktypeAct->addItems(QStringList() << "genetic" << "fixed" << "back-prop" );
// 	connect(blocktypeAct, SIGNAL(currentIndexChanged(int)), this, SLOT(changeblocktype(int)) );
// 	m_toolBar->addWidget( blocktypeAct );
//
// 	neurontypeAct = new QComboBox( m_toolBar );
// 	neurontypeAct->addItems( QStringList() << "neuron type");
// 	neurontypeAct->addItems( QStringList() << "sigmoid" << "integrator" << "binary" << "sigmoid flat" );
// 	connect( neurontypeAct, SIGNAL(currentIndexChanged(int)), this, SLOT(set_neurontype(int)) );
// 	m_toolBar->addWidget( neurontypeAct );
//
// 	m_toolBar->addAction(display_labelAct);
// 	m_toolBar->addAction(display_weightAct);
// 	m_toolBar->addAction(display_deltaAct);
// 	m_toolBar->addAction(display_biasAct);
// 	m_toolBar->addAction(display_gainAct);
// 	m_toolBar->addAction(set_neurondisplayAct);
// 	m_toolBar->addAction(set_lesionAct);
// 	m_toolBar->addAction(set_neuronbiasAct);
// 	m_toolBar->addAction(set_neurongainAct);
// 	m_toolBar->addAction(add_ublockAct);
// 	m_toolBar->addAction(add_cblockAct);
// 	m_toolBar->addAction(add_gblockAct);
// 	m_toolBar->addAction(erase_Act);
// }
//
// void NetworkDialog::createActions()
// {
//
//
// 	openAct = new QAction(QIcon(":/evorobot/open.png"), tr("&Open"), this);
//
//
//     openAct->setStatusTip(tr("Open a network architecture or a phenotype file"));
//     connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
//
//     saveAct = new QAction(QIcon(":/evorobot/save.png"), tr("&Save"), this);
//     saveAct->setStatusTip(tr("Save the network architecture or the phenotype from a file"));
//     connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
//
//     set_neurondisplayAct = new QAction(QIcon(":/evorobot/show.png"), tr("&Set/Unset neurons to be displayed as graph"), this);
//     set_neurondisplayAct->setShortcut(tr("Ctrl+N"));
//     set_neurondisplayAct->setStatusTip(tr("Select/Deselect neurons to be displayed as graph"));
//     connect(set_neurondisplayAct, SIGNAL(triggered()), this, SLOT(set_neurondisplay()));
//
//     set_neurongainAct = new QAction(QIcon(":/evorobot/gain.png"), tr("&Add/Remove neurons gain"), this);
//     set_neurongainAct->setShortcut(tr("Ctrl+G"));
//     set_neurongainAct->setStatusTip(tr("Add/Remove neurons gain"));
//     connect(set_neurongainAct, SIGNAL(triggered()), this, SLOT(set_neurongain()));
//
//     set_neuronbiasAct = new QAction(QIcon(":/evorobot/bias.png"), tr("&Add/Remove neurons biases"), this);
//     set_neuronbiasAct->setShortcut(tr("Ctrl+B"));
//     set_neuronbiasAct->setStatusTip(tr("Add/Remove neurons bias"));
//     connect(set_neuronbiasAct, SIGNAL(triggered()), this, SLOT(set_neuronbias()));
//
//     set_lesionAct = new QAction(QIcon(":/evorobot/lesion.png"), tr("&Lesion/Restore neurons"), this);
//     set_lesionAct->setShortcut(tr("Ctrl+B"));
//     set_lesionAct->setStatusTip(tr("Lesion/Restore neurons"));
//     connect(set_lesionAct, SIGNAL(triggered()), this, SLOT(set_lesion()));
//
//     display_labelAct = new QAction(QIcon(":/evorobot/label.png"), tr("&Display/Undisplay neuron labels"), this);
//     display_labelAct->setShortcut(tr("Ctrl+L"));
//     display_labelAct->setStatusTip(tr("Display neuron labels"));
//     connect(display_labelAct, SIGNAL(triggered()), this, SLOT(display_label()));
//
//     display_weightAct = new QAction(QIcon(":/evorobot/weight.png"), tr("&Display/Undisplay weights"), this);
//     display_weightAct->setShortcut(tr("Ctrl+W"));
//     display_weightAct->setStatusTip(tr("Display the incoming and outcoming weight of a neuron"));
//     connect(display_weightAct, SIGNAL(triggered()), this, SLOT(display_weight()));
//
//     display_deltaAct = new QAction(QIcon(":/evorobot/ddelta.png"), tr("&Display/Undisplay neurons delta"), this);
//     display_deltaAct->setShortcut(tr("Ctrl+Y"));
//     display_deltaAct->setStatusTip(tr("Display neurons delta"));
//     connect(display_deltaAct, SIGNAL(triggered()), this, SLOT(display_delta()));
//
//     display_biasAct = new QAction(QIcon(":/evorobot/dbias.png"), tr("&Display/Undisplay biases"), this);
//     display_biasAct->setShortcut(tr("Ctrl+Z"));
//     display_biasAct->setStatusTip(tr("Display neurons biases"));
//     connect(display_biasAct, SIGNAL(triggered()), this, SLOT(display_bias()));
//
//     display_gainAct = new QAction(QIcon(":/evorobot/dgain.png"), tr("&Display/Undisplay gains"), this);
//     display_gainAct->setShortcut(tr("Ctrl+G"));
//     display_gainAct->setStatusTip(tr("Display neurons gains"));
//     connect(display_gainAct, SIGNAL(triggered()), this, SLOT(display_gain()));
//
//     erase_Act = new QAction(QIcon(":/evorobot/erase.png"), tr("&Erase"), this);
//     erase_Act->setShortcut(tr("Ctrl+E"));
//     erase_Act->setStatusTip(tr("Erase all or selected properties"));
//     connect(erase_Act, SIGNAL(triggered()), this, SLOT(erase()));
//
//     add_ublockAct = new QAction(QIcon(":/evorobot/addublock.png"), tr("&Add Update Block"), this);
//     add_ublockAct->setShortcut(tr("Ctrl+A"));
//     add_ublockAct->setStatusTip(tr("add an update block"));
//     connect(add_ublockAct, SIGNAL(triggered()), this, SLOT(add_ublock()));
//
//     add_cblockAct = new QAction(QIcon(":/evorobot/addblock.png"), tr("&Add Connection Block"), this);
//     add_cblockAct->setShortcut(tr("Ctrl+A"));
//     add_cblockAct->setStatusTip(tr("add a connection block"));
//     connect(add_cblockAct, SIGNAL(triggered()), this, SLOT(add_cblock()));
//
//     add_gblockAct = new QAction(QIcon(":/evorobot/gainblock.png"), tr("&Add a gain block"), this);
//     add_gblockAct->setShortcut(tr("Ctrl+G"));
//     add_gblockAct->setStatusTip(tr("add a gain block"));
//     connect(add_gblockAct, SIGNAL(triggered()), this, SLOT(add_gblock()));
//
//     increasevAct = new QShortcut(tr("+"), this);
//     connect(increasevAct, SIGNAL(activated()), this, SLOT(increasev()));
//
//     decreasevAct = new QShortcut(tr("-"), this);
//     connect(decreasevAct, SIGNAL(activated()), this, SLOT(decreasev()));
//
//     //connect(rendNetwork, SIGNAL(selectedneuronsChanged()), this, SLOT(updatetoolb()) );
//
// }
//
// // return the current selected block (-1 in case of no selection)
// int NetworkDialog::getcblock()
// {
//    return(cblockAct->currentIndex() - 1);
// }
//
//
// // update the neuron type combo box status
// // this method is triggered when the user select a neuron
// void NetworkDialog::updatetoolb()
// {
//
//   // we set the index of the neuron-type combo box to that of the first selected neuron
//   // this trigger the set_neurontype function
//   if (cneuronn == 1)
//     {
//      neurontypeAct->setCurrentIndex(enet->neurontype[cneuron[0]]+1);
//     }
//
// }
//
// // update the block type combo box status
// // this function is triggered when a new block is selected
// void NetworkDialog::updateblocktype()
// {
//
// 	int cb;
//
//     cb = getcblock();
//
//     // we set the type combobox index on the basis of the type current selected block
//     blocktypeAct->setCurrentIndex(enet->net_block[cb][5]+1);
//
// }
//
//
// // modify the type of a block
// // this function is triggered by the user
// void NetworkDialog::changeblocktype(int t)
// {
//
// 	int cb;
// 	int oldt;
//
// 	cb = getcblock();
//
// 	if (t > 0 && cb > 0 /*&& blocktypeauto == 0 */) {
// 		oldt = enet->net_block[cb][5];
// 		enet->net_block[cb][5] = t - 1;
// 		if (oldt != t - 1) {
// 			statusb->setText(QString("The type of the selected block has been modified"));
// 		}
// 	}
// }
//
// // define or undefine neurons to be displayed
// void NetworkDialog::set_neurondisplay()
// {
//    int i;
//
//    if (cneuronn == 1)
//    {
//      if (enet->neurondisplay[cneuron[0]] == 1)
//        enet->neurondisplay[cneuron[0]] = 0;
//       else
//        enet->neurondisplay[cneuron[0]] = 1;
//    }
//    if (cneuronn == 2)
//    {
//      if (enet->neurondisplay[cneuron[0]] == 1)
//       for (i=cneuron[0]; i <= cneuron[1]; i++)
//        enet->neurondisplay[i] = 0;
//      else
//       for (i=cneuron[0]; i <= cneuron[1]; i++)
//        enet->neurondisplay[i] = 1;
//    }
//    if (cneuronn < 1 || cneuronn > 2)
//      warning("you should select one neuron or one block of neurons first");
//    else
// 	statusb->setText(QString("Neurons to be monitored have been updated"));
//
//    cneuronn = 0;
//    rendNetwork->update();
//
// }
// // change neurons' type
// void NetworkDialog::set_neurontype(int t)
// {
//
// 	int oldt;
//
// 	if (t > 0) {  // nothing is done if combobox is reset to the "neuron type" title slot
// 		oldt = enet->neurontype[cneuron[0]];
// 		if (cneuronn == 1) {
// 			enet->neurontype[cneuron[0]] = t-1;
// 		}
// 		if (cneuronn == 2) {
// 			for (int i=cneuron[0]; i <= cneuron[1]; i++) {
// 				enet->neurontype[i] = t-1;
// 			}
// 		}
// 		if (cneuronn < 1 || cneuronn > 2)
// 			warning("you should select one neuron or one block of neurons first");
// 		else {
// 			if (oldt != enet->neurontype[cneuron[0]])
// 				statusb->setText(QString("Neurons' type updated"));
// 		}
// 		cneuronn = 0;
// 		rendNetwork->update();
// 	}
// }
//
//
// // manage actions associated to the selection of a block
// // the signal trigger the block type update function
// void NetworkDialog::selblock_changed(int t)
// {
// 	if (t > 0)
// 		statusb->setText(QString("Block selected"));
// 	emit selectedblockChanged();
// 	rendNetwork->update();
// }
//
// // define or undefine neurons gain
// void NetworkDialog::set_neurongain()
// {
//    int i;
//
//    if (cneuronn < 1 || cneuronn > 2)
//    {
//      warning("you should select one neuron or one block of neurons first");
//    }
//    else
//    {
//    if (cneuronn == 1)
//    {
//      if (enet->neurongain[cneuron[0]] == 1)
// 	 {
//        enet->neurongain[cneuron[0]] = 0;
//        statusb->setText(QString("Neuron's gain eliminated"));
// 	 }
//       else
// 	  {
//        enet->neurongain[cneuron[0]] = 1;
//        statusb->setText(QString("Neuron's gain added"));
// 	  }
//    }
//    if (cneuronn == 2)
//    {
//      if (enet->neurongain[cneuron[0]] == 1)
// 	 {
//       for (i=cneuron[0]; i <= cneuron[1]; i++)
//         enet->neurongain[i] = 0;
//       statusb->setText(QString("Neurons' gain eliminated"));
// 	 }
//      else
// 	 {
//       for (i=cneuron[0]; i <= cneuron[1]; i++)
//        enet->neurongain[i] = 1;
//       statusb->setText(QString("Neurons' gain added"));
//
// 	 }
//    }
//    }
//
//     cneuronn = 0;
//     rendNetwork->update();
//
// }
//
//
// // define or undefine neurons bias
// void NetworkDialog::set_neuronbias()
// {
//    int i;
//
//    if (cneuronn < 1 || cneuronn > 2)
//    {
//      warning("you should select one neuron or one block of neurons first");
//    }
//    else
//    {
//    if (cneuronn == 1)
//    {
//      if (enet->neuronbias[cneuron[0]] == 1)
// 	 {
//        enet->neuronbias[cneuron[0]] = 0;
//        statusb->setText(QString("Neuron's bias eliminated"));
// 	 }
//       else
// 	  {
//        enet->neuronbias[cneuron[0]] = 1;
//        statusb->setText(QString("Neuron's bias added"));
// 	  }
//    }
//    if (cneuronn == 2)
//    {
//      if (enet->neuronbias[cneuron[0]] == 1)
// 	 {
//       for (i=cneuron[0]; i <= cneuron[1]; i++)
//        enet->neuronbias[i] = 0;
//        statusb->setText(QString("Neurons' bias eliminated"));
// 	 }
//      else
// 	 {
//       for (i=cneuron[0]; i <= cneuron[1]; i++)
//        enet->neuronbias[i] = 1;
//        statusb->setText(QString("Neurons' bias added"));
// 	 }
//    }
//    cneuronn = 0;
//    rendNetwork->update();
//    }
//
// }
//
// /*
//  * Lesion/unlesion neurons
//  */
// void NetworkDialog::set_lesion()
// {
//
//     if (enet->neuronlesions == 0)
// 	{
// 	 if (cneuronn == 1)
// 	 {
// 		enet->neuronlesion[cneuron[0]]=!enet->neuronlesion[cneuron[0]];
// 		cneuronn = 0;
//         enet->neuronlesions = 1;
// 		openMixer();
//         statusb->setText(QString("Neuron's lesioned"));
// 		rendNetwork->update();
// 	 }
// 	  else
// 	   if (cneuronn == 2)
// 	     {
// 		  for(int ls = cneuron[0]; ls <= cneuron[1]; ls++)
// 		    {
// 			enet->neuronlesion[ls] = !enet->neuronlesion[ls];
//             enet->neuronlesions++;
// 		    }
// 		  cneuronn = 0;
// 		  openMixer();
//           statusb->setText(QString("Neurons' lesioned"));
// 		  rendNetwork->update();
// 	     }
// 	     else
// 		 {
// 		  warning("you should select one neuron or one block of neurons first");
// 	     }
//     }
// 	else
//     {
//       enet->neuronlesions = 0;
//       for (int i = 0; i < enet->nneurons; i++)
// 	   {
// 		enet->neuronlesion[i]=false;
// 		enet->neuronlesionVal[i]=0.0;
// 	   }
// 	  mixerDialog->hide();
//       statusb->setText(QString("Neurons un-lesioned"));
//       rendNetwork->update();
// 	}
// }
//
// // Display neurons label
// void NetworkDialog::display_label()
// {
//
//    pseudomode = 0;
//    cneuronn = 0;
//    pseudo_activate_net();
//    statusb->setText(QString("Neurons labels displayed"));
//    rendNetwork->update();
//
// }
//
//
//
// // Display/Undisplay neurons delta
// void NetworkDialog::display_delta()
// {
//
// 	if (cneuronn == 1 || cneuronn == 2)
// 	{
// 	pseudomode = 1;
// 	pseudo_activate_net();
// 	openMixer();
// 	cneuronn = 0;
// 	statusb->setText(QString("Neurons' timeconstants weight displayed"));
// 	rendNetwork->update();
// 	}
// 	else
// 	{
// 	  warning("you should select one neuron or one block of neurons first");
// 	}
// }
//
//
// // Display/Undisplay the weights of a neuron
// void NetworkDialog::display_weight()
// {
//
//    if (cneuronn == 4)
//      {
//       pseudomode = 2;
//       pseudo_activate_net();
// 	  statusb->setText(QString("Connections weights displayed"));
// 	  openMixer();
//       cneuronn = 0;
//       rendNetwork->update();
//      }
//      else
//      {
//       warning("you should select the receiving and sending block of neurons first (4 neurons)");
//      }
// }
//
// // Display/Undisplay neurons biases
// void NetworkDialog::display_bias()
// {
//
//    if (cneuronn == 1 || cneuronn == 2)
//      {
//       pseudomode = 3;
//       pseudo_activate_net();
// 	  statusb->setText(QString("Biases weight displayed"));
//       rendNetwork->update();
// 	  openMixer();
//      }
//      else
//      {
//       warning("you should select one neuron or one block of neurons first");
//      }
//
// }
//
// // Display/Undisplay neurons gains
// void NetworkDialog::display_gain()
// {
//
//
//    if (cneuronn == 1 || cneuronn == 2)
//      {
//       pseudomode = 4;
//       pseudo_activate_net();
// 	  openMixer();
//       cneuronn = 0;
// 	  statusb->setText(QString("Neurons' gain weight displayed"));
//       rendNetwork->update();
//      }
//      else
//      {
//       warning("you should select one neuron or one block of neurons first");
//      }
// }
//
//
// // Erase a selected block or all blocks (when no block is selected)
// void NetworkDialog::erase()
// {
//
//    int cblock;
//    int i, ii;
//    int bb;
//
//
//    cblock = getcblock();
//
//    if (cblock < 0)
//    {
//     // we erase all blocks and reset neurons' properties
//     enet->net_nblocks = 0;
//     cblockAct->clear();
//     cblockAct->addItems(QStringList() << "blocks list");
//     for(i=0;i < enet->nneurons; i++)
//        {
//          enet->neuronbias[i] = 0;
//          enet->neurontype[i] = 0;
// 		 enet->neurongain[i] = 0;
//          enet->neurondisplay[i] = 1;
//        }
//     statusb->setText(QString("All blocks erased and neurons' properties reset"));
//     }
//    else
//    {
//     // we erase the selected block
//      cblockAct->removeItem(cblock+1);
//      cblockAct->setCurrentIndex(0);
//      for (bb=cblock; bb < (enet->net_nblocks - 1); bb++)
//         for (ii=0; ii < 6; ii++)
//            enet->net_block[bb][ii] = enet->net_block[bb+1][ii];
//      enet->net_nblocks--;
//      statusb->setText(QString("Selected block erased"));
//    }
//    rendNetwork->update();
// }
//
//
// // Add an update block
// void NetworkDialog::add_ublock()
// {
//
//   int i,ii;
//   char stblock[1024];
//   int cblock;
//
//   if (cneuronn != 1 && cneuronn != 2)
//     {
//       warning("you should select a a lock (2 neurons) first");
// 	  return;
//     }
//
//   cblock = getcblock(); // we add the new block before the selected block
//   if (cblock < 0)
//     cblock = enet->net_nblocks; // or we add it at the end of the block list
//
//   if (cneuronn == 1)
//     {
// 	for(i=enet->net_nblocks; i > cblock; i--)
// 		for(ii=0; ii<6; ii++)
// 			enet->net_block[i][ii] = enet->net_block[i-1][ii];
//      enet->net_block[cblock][0] = 1;
//      enet->net_block[cblock][1] = cneuron[0];
//      enet->net_block[cblock][2] = 1;
//      enet->net_block[cblock][3] = 0;
//      enet->net_block[cblock][4] = 0;
//      enet->net_block[cblock][5] = 0;
// 	 block_name(cblock, stblock);
// 	 cblockAct->insertItems(cblock+1, QStringList() << stblock);
//      enet->net_nblocks++;
//      cneuronn = 0;
//     }
//
//   if (cneuronn == 2)
//     {
// 	for(i=enet->net_nblocks; i > cblock; i--)
// 		for(ii=0; ii<6; ii++)
// 			enet->net_block[i][ii] = enet->net_block[i-1][ii];
//      enet->net_block[cblock][0] = 1;
//      enet->net_block[cblock][1] = cneuron[0];
//      enet->net_block[cblock][2] = cneuron[1] - cneuron[0] + 1;
//      enet->net_block[cblock][3] = 0;
//      enet->net_block[cblock][4] = 0;
//      enet->net_block[cblock][5] = 0;
// 	 block_name(cblock, stblock);
// 	 cblockAct->insertItems(cblock+1, QStringList() << stblock);
//      enet->net_nblocks++;
//      cneuronn = 0;
//     }
//
//    cblockAct->setCurrentIndex(0);
//    statusb->setText(QString("Update block added"));
//    rendNetwork->update();
// }
//
//
// // Add a connection block
// void NetworkDialog::add_cblock()
// {
//
//   int i,ii;
//   char stblock[1024];
//   int cblock;
//
//
//   if (cneuronn != 4)
//     {
//       warning("you should select the receiving and sending blocks (4 neurons) first");
//       return;
//     }
//
//   cblock = getcblock(); // we add the new block before the selected block
//   printf("cblock %d\n", cblock);
//   if (cblock < 0)
//     cblock = enet->net_nblocks; // or we add it at the end of the block list
//
//
//   if (cneuronn == 4)
//     {
// 	 for(i=enet->net_nblocks; i > cblock; i--)
// 		for(ii=0; ii<6; ii++)
// 			enet->net_block[i][ii] = enet->net_block[i-1][ii];
//      enet->net_block[cblock][0] = 0;
//      enet->net_block[cblock][1] = cneuron[0];
//      enet->net_block[cblock][2] = cneuron[1] - cneuron[0] + 1;
//      enet->net_block[cblock][3] = cneuron[2];
//      enet->net_block[cblock][4] = cneuron[3] - cneuron[2] + 1;
//      enet->net_block[cblock][5] = 0;
// 	 block_name(cblock, stblock);
// 	 cblockAct->insertItems(cblock+1, QStringList() << stblock);
//      enet->net_nblocks++;
//      cneuronn = 0;
//     }
//
//    cblockAct->setCurrentIndex(0);
//    statusb->setText(QString("Connection block added"));
//    rendNetwork->update();
// }
//
// // Add a gain block
// void NetworkDialog::add_gblock()
// {
//
//   int i,ii;
//   char stblock[1024];
//   int cblock;
//
//   if (cneuronn != 2 && cneuronn != 3)
//     {
//       warning("you should select a block (2 neurons) and eventually a third neuron first");
//       return;
//     }
//
//   cblock = getcblock(); // we add the new block before the selected block
//   if (cblock < 0)
//     cblock = enet->net_nblocks; // or we add it at the end of the block list
//
//
//   // a block with 2 selected neurons operates by making the gain of all the neurons of the block equal to that of the first neuron of the block
//   if (cneuronn == 2)
//     {
// 	 for(i=enet->net_nblocks; i > cblock; i--)
// 		for(ii=0; ii<6; ii++)
// 			enet->net_block[i][ii] = enet->net_block[i-1][ii];
//      enet->net_block[cblock][0] = 2;
//      enet->net_block[cblock][1] = cneuron[0];
//      enet->net_block[cblock][2] = cneuron[1] - cneuron[0] + 1;
//      enet->net_block[cblock][3] = 0;
//      enet->net_block[cblock][4] = 0;
//      enet->net_block[cblock][5] = 0;
// 	 block_name(cblock, stblock);
// 	 cblockAct->insertItems(cblock+1, QStringList() << stblock);
//      enet->net_nblocks++;
//      cneuronn = 0;
//     }
//   // a block with 3 selected neurons operates by making the gain the neurons of the block equal the to activation state of the 3rd neuron at time t-1
//   if (cneuronn == 3)
//     {
// 	 for(i=enet->net_nblocks; i > cblock; i--)
// 		for(ii=0; ii<6; ii++)
// 			enet->net_block[i][ii] = enet->net_block[i-1][ii];
//      enet->net_block[cblock][0] = 3;
//      enet->net_block[cblock][1] = cneuron[0];
//      enet->net_block[cblock][2] = cneuron[1] - cneuron[0] + 1;
//      enet->net_block[cblock][3] = cneuron[2];
//      enet->net_block[cblock][4] = 0;
//      enet->net_block[cblock][5] = 0;
// 	 block_name(cblock, stblock);
// 	 cblockAct->insertItems(cblock+1, QStringList() << stblock);
//      enet->net_nblocks++;
//      cneuronn = 0;
//     }
//
//    cblockAct->setCurrentIndex(0);
//    statusb->setText(QString("Gain block added"));
//    rendNetwork->update();
// }
//
//
// /*
//  * open a .net or .phe file
//  */
// void NetworkDialog::open()
// {
//
//     char *f;
//     QByteArray filen;
//     char filename[256];
//
//
//     QString fileName = QFileDialog::getOpenFileName(this,
//             "Choose a filename to open",
//                     "",
//                     "*.net *.phe");
//
//
//     if (fileName.endsWith("net"))
//     {
//       filen = fileName.toLatin1();
//       f = filen.data();
//       strcpy(filename, f);
//       enet->load_net_blocks(filename, 0);
//       statusb->setText(QString("File %1 loaded").arg(filename));
//     }
//     else
//      if (fileName.endsWith("phe"))
//      {
//       filen = fileName.toLatin1();
//       f = filen.data();
//       strcpy(filename, f);
//       enet->load_net_blocks(filename, 1);
//       statusb->setText(QString("File %1 loaded").arg(filename));
//      }
//
// }
//
// /*
//  * save a .net or .phe file
//  */
// void NetworkDialog::save()
// {
//
//     char *f;
//     QByteArray filen;
//     char filename[256];
//
//
//     QString fileName = QFileDialog::getSaveFileName(this,
//             "Choose a filename to save",
//                     "",
//                     "*.net *.phe");
//
//
//     if (fileName.endsWith("net"))
//     {
//       filen = fileName.toLatin1();
//       f = filen.data();
//       strcpy(filename, f);
//       enet->save_net_blocks(filename, 0);
//       statusb->setText(QString("File %1 saved").arg(filename));
//
//     }
//     else
//      if (fileName.endsWith("phe"))
//      {
//       filen = fileName.toLatin1();
//       f = filen.data();
//       strcpy(filename, f);
//       enet->save_net_blocks(filename, 1);
//       statusb->setText(QString("File %1 saved").arg(filename));
//      }
//
// }
//
// // Increase a selected value
// void NetworkDialog::increasev()
// {
//    float **gp;
//    float *v;
//
//
//    // when 2 neurons are selected we assume that the user want to change a connection weight
//    if (cneuronn == 2)
//      {
//       pseudomode = 2;
//       pseudo_activate_net();
//       if (enet->nselected == 1)
//       {
//        gp = enet->selectedp;
//        v = *gp;
//        *v += ((enet->wrange * 2.0f) / 256.0f);
//        pseudo_activate_net();
//        rendNetwork->update();
//       }
//      }
//    // when 1 neuron is selected we modify the bias of the neuron,
//    // or the delta or the gain of the neuron (if the user previously pushed the delta or gain buttons, respectively)
//    if (cneuronn == 1)
//      {
//       if (pseudomode != 1 && pseudomode != 4)
//         pseudomode = 3;
//       pseudo_activate_net();
//       if (enet->nselected == 1)
//       {
//        gp = enet->selectedp;
//        v = *gp;
//        *v += ((enet->wrange * 2.0f) / 256.0f);
//        pseudo_activate_net();
//        rendNetwork->update();
//       }
//      }
//
// }
//
// // Decrease a selected value
// void NetworkDialog::decreasev()
// {
//
//     float **gp;
//     float *v;
//
//
//    // when 2 neurons are selected we assume that the user want to change a connection weight
//    if (cneuronn == 2)
//      {
//       pseudomode = 2;
//       pseudo_activate_net();
//       if (enet->nselected == 1)
//       {
//        gp = enet->selectedp;
//        v = *gp;
//        *v -= ((enet->wrange * 2.0f) / 256.0f);
//        pseudo_activate_net();
//        rendNetwork->update();
//       }
//      }
//    // when 1 neuron is selected we modify the bias of the neuron,
//    // or the delta or the gain of the neuron (if the user previously pushed the delta or gain buttons, respectively)
//    if (cneuronn == 1)
//      {
//       if (pseudomode != 1 && pseudomode != 4)
//         pseudomode = 3;
//       pseudo_activate_net();
//       if (enet->nselected == 1)
//       {
//        gp = enet->selectedp;
//        v = *gp;
//        *v -= ((enet->wrange * 2.0f) / 256.0f);
//        pseudo_activate_net();
//        rendNetwork->update();
//       }
//      }
//
// }
//
// // It return the name/description of an existing block
// void NetworkDialog::block_name(int b, char *st)
// {
//         switch(enet->net_block[b][0])
// 		{
// 			case 0:
// 				sprintf(st,"c) %s_%s  %s_%s", enet->neuronl[enet->net_block[b][1]], enet->neuronl[enet->net_block[b][1]+enet->net_block[b][2]-1],enet->neuronl[enet->net_block[b][3]],enet->neuronl[enet->net_block[b][3]+enet->net_block[b][4]-1]);
// 			break;
// 			case 1:
// 	            sprintf(st,"u) %s_%s", enet->neuronl[enet->net_block[b][1]], enet->neuronl[enet->net_block[b][1]+enet->net_block[b][2]-1]);
// 			break;
// 			case 2:
// 				sprintf(st,"g) %s_%s", enet->neuronl[enet->net_block[b][1]], enet->neuronl[enet->net_block[b][1]+enet->net_block[b][2]-1]);
// 			break;
// 			case 3:
// 				sprintf(st,"g) %s_%s  %s", enet->neuronl[enet->net_block[b][1]], enet->neuronl[enet->net_block[b][1]+enet->net_block[b][2]-1],enet->neuronl[enet->net_block[b][3]]);
// 			break;
// 			default:
// 				sprintf(st,"undefined block type");
// 			break;
// 		}
//
// }
//
// /*
//  * This function replace the current neuron labels string with the user's selected parameter:
//  * (timeconstant, weight, bias, gain). Selected parameters are also stored in the selectedp
//  * pointer list.
//  */
// void
// NetworkDialog::pseudo_activate_net()
//
// {
//     int i;
//     int t;
//     int b;
//     float delta;
//     float gain;
// 	float bias;
//     float **gp;
//     int ddelta=0;
//     int dweight=0;
//     int dbias=0;
//     int dgain=0;
// 	float *p;
//
//     switch(pseudomode)
//      {
//        case 1:
//          ddelta = 1;
//          break;
//        case 2:
//          dweight = 1;
//          break;
//        case 3:
//          dbias = 1;
//          break;
//        case 4:
//          dgain = 1;
//          break;
//      }
//
// 	 p = freep;
//      gp = enet->selectedp;
//      enet->nselected = 0;
//
//      for(i=0;i < enet->nneurons;i++)
//       strcpy(neuroncl[i],enet->neuronl[i]);
//
//      //for(i=0;i < enet->nneurons;i++)
//      // updated[i] = 0;
//
//      // gain (from the first to the second selected neuron)
//      for(i=0;i < enet->nneurons;i++)
//      {
//        if (enet->neurongain[i] == 1)
//        {
//          if (dgain > 0 && ((cneuronn == 1 && i == cneuron[0]) || (cneuronn == 2 && i >= cneuron[0] && i <= cneuron[1])))
//           {
//            gain = (fabs((double) *p) / enet->wrange) * enet->grange;
//            sprintf(neuroncl[i],"%.1f",gain);
//            *gp = p;
//            gp++;
//            enet->nselected++;
//           }
//           p++;
//        }
//      }
//
//      // biases (from the first to the second selected neuron)
//      for(i=0;i < enet->nneurons;i++)
//      {
//        if (enet->neuronbias[i] == 1)
//        {
//          if (dbias > 0 && ((cneuronn == 1 && i == cneuron[0]) || (cneuronn == 2 && i >= cneuron[0] && i <= cneuron[1])))
//           {
//             bias = (fabs((double) *p) / enet->wrange) * enet->brange;
// 			sprintf(neuroncl[i],"%.1f",bias);
//            *gp = p;
//            gp++;
//            enet->nselected++;
//           }
//           p++;
//        }
//      }
//
//      // blocks
//      for (b=0; b < enet->net_nblocks; b++)
//      {
//        // connection block
//        if (enet->net_block[b][0] == 0)
//        {
//          for(t=enet->net_block[b][1]; t < enet->net_block[b][1] + enet->net_block[b][2];t++)
//           for(i=enet->net_block[b][3]; i < enet->net_block[b][3] + enet->net_block[b][4];i++)
//           {
//            // weights (4 neurons, receiving block, sending block)
//            if (dweight == 1 && cneuronn == 4 && (t >= cneuron[0] && t <= cneuron[1]) && (i >= cneuron[2] && i <= cneuron[3]))
//              {
// 		      // values can be visualized in the label only if a single receiving neuron has been selected
// 			  if (cneuron[0] == cneuron[1])
//                 sprintf(neuroncl[i],"%.1f",*p);
//               *gp = p;
//               gp++;
//               enet->nselected++;
//              }
//             p++;
//           }
//        }
//        // update block
//        if (enet->net_block[b][0] == 1)
//        {
//          for(t=enet->net_block[b][1]; t < (enet->net_block[b][1] + enet->net_block[b][2]); t++)
//           {
//           //updated[t] +=1;
//           if (enet->neurontype[t] != 1)
//            {
//             ;
//            }
//            else
//            {
//             // timeconstant (from the first to the second selected neuron)
//             delta = (float) (fabs((double) *p) / enet->wrange);
//             if (ddelta > 0 && ((cneuronn == 1 && t == cneuron[0]) || (cneuronn == 2 && t >= cneuron[0] && t <= cneuron[1])))
//              {
//               sprintf(neuroncl[t],"%.1f",delta);
//               *gp = p;
//               gp++;
//               enet->nselected++;
//              }
//             p++;
//            }
//           }
//        }
//      }
//
//
// }
//
// /*
//  * Mixer for lesioned or freezed neurons
//  */
// void NetworkDialog::openMixer()
// {
//
// 	//mixerDialog=new MixerDialog(this->enet, this);
// 	mixerDialog->setUpMixer();
// 	mixerDialog->show();
// 	rendNetwork->update();
//
// }
//
//
//
// // -----------------------------------------------------
// // Widget RendNetwork
// // -----------------------------------------------------
//
// RendNetwork::RendNetwork( NetworkDialog* networkDialog, QWidget *parent)
//     : QWidget(parent)
// {
//     shape = Polygon;
//     antialiased = false;
//     pixmap.load(":/evorobot/qt-logo.png");
//
//     setBackgroundRole(QPalette::Base);
//     this->networkDialog = networkDialog;
//
// 	rnmousex = 20000;
//     rnmousey = 30000;
// }
//
// QSize RendNetwork::minimumSizeHint() const
// {
//     return QSize(250, 250);
// }
//
// QSize RendNetwork::sizeHint() const
// {
//    return QSize(550, 500);
// }
//
// void RendNetwork::setShape(Shape shape)
// {
//     this->shape = shape;
//     update();
// }
//
// void RendNetwork::setPen(const QPen &pen)
// {
//     this->pen = pen;
//     update();
// }
//
//
//
// void RendNetwork::setBrush(const QBrush &brush)
// {
//     this->brush = brush;
//     update();
// }
//
// void RendNetwork::setAntialiased(bool antialiased)
// {
//     this->antialiased = antialiased;
//     update();
// }
//
// void RendNetwork::setTransformed(bool transformed)
// {
//     this->transformed = transformed;
//     update();
// }
//
//
// void RendNetwork::paintEvent(QPaintEvent *)
// {
//
//
//    int    i;
//    int    t,b;
//    int    sx,sy,dx,dy;
//    float  *p;
//    int     cr,cb,cg;
//    int     wid = 1;
//    float   w;
//    int     selectedneurons[2];
//    QPainter painter(this);
//
//    QRect labelxy(0,0,30,20);          // neuron labels
//    QPoint pxy;
//    painter.setPen(pen);
//    QPen pen(Qt::black, 1);                 // black solid line, 1 pixels wide
//    painter.setPen(pen);
//    selectedneurons[0] = selectedneurons[1] = -1;
//    // set the scale
//    if (networkDialog->enet->drawnxmax > 0 && networkDialog->enet->drawnymax > 0)
//     {
//      networkDialog->cscaley = height() / (float) networkDialog->enet->drawnymax;
//      networkDialog->cscalex = width() / (float) networkDialog->enet->drawnxmax;
//      painter.scale(networkDialog->cscalex, networkDialog->cscaley);
//     }
//     else
//     {
//      painter.scale(1.0,1.0);
//     }
//     painter.setPen(Qt::gray);
// 	// extract gain and biases
//     p  = networkDialog->enet->freep;
//     for(i=0;i < networkDialog->enet->nneurons;i++)
//      {
//       if (networkDialog->enet->neurongain[i] == 1)
//        p++;
//      }
//     for(i=0;i < networkDialog->enet->nneurons;i++)
//      {
//       if (networkDialog->enet->neuronbias[i] == 1)
//        {
//         networkDialog->biases[i] = *p;
//         p++;
//        }
//      }
// 	// draw connections
//     for (i=0; i < networkDialog->enet->net_nblocks; i++)
//     {
//       if (networkDialog->enet->net_block[i][0] == 0)
//         for (t=networkDialog->enet->net_block[i][1]; t < (networkDialog->enet->net_block[i][1] + networkDialog->enet->net_block[i][2]); t++)
//           for (b=networkDialog->enet->net_block[i][3]; b < (networkDialog->enet->net_block[i][3] + networkDialog->enet->net_block[i][4]); b++)
//            {
//
// 		     if (networkDialog->getcblock() == i)
//                wid = 2;
//               else
//                wid = 1;
//              w = *p;
// 			 if (w != networkDialog->enet->DEFAULT_VALUE)
// 			  {
//                if (w > networkDialog->enet->wrange)
//                  w = networkDialog->enet->wrange;
//                if (w < (0.0 - networkDialog->enet->wrange))
//                  w = 0.0 - networkDialog->enet->wrange;
//                cr = cg = cb = 255;
//                if (w > 0)
//                 {
//                   cg -= ((w / networkDialog->enet->wrange) * 255);
//                   cb -= ((w / networkDialog->enet->wrange) * 255);
//                 }
//                else
//                 {
//                   cg += ((w / networkDialog->enet->wrange) * 255);
//                   cr += ((w / networkDialog->enet->wrange) * 255);
//                 }
// 			   }
// 			 else
// 			 {
//                cr=0;
// 			   cg=185;
// 			   cb=0;
// 			 }
//
//            painter.setPen(QPen(QColor(cr,cg,cb,255), wid, Qt::SolidLine));
//             p++;
//             if (abs(networkDialog->enet->neuronxy[b][1] - networkDialog->enet->neuronxy[t][1]) > 20)
//               painter.drawLine((float) networkDialog->enet->neuronxy[t][0], (float) networkDialog->enet->neuronxy[t][1], (float) networkDialog->enet->neuronxy[b][0], (float) networkDialog->enet->neuronxy[b][1]);
//              else
//              {
//               dx = abs(networkDialog->enet->neuronxy[t][0] - networkDialog->enet->neuronxy[b][0]);
//               dy = abs(networkDialog->enet->neuronxy[t][1] - networkDialog->enet->neuronxy[b][1]);
//               if (networkDialog->enet->neuronxy[t][0] < networkDialog->enet->neuronxy[b][0])
//                 sx = networkDialog->enet->neuronxy[t][0];
//                else
//                 sx = networkDialog->enet->neuronxy[b][0];
//               if (networkDialog->enet->neuronxy[t][1] < networkDialog->enet->neuronxy[b][1])
//                 sy = networkDialog->enet->neuronxy[t][1];
//                else
//                 sy = networkDialog->enet->neuronxy[b][1];
//               painter.drawArc((float) sx,(float) (sy-20),(float) dx, (float) (dy+40), 0 * 16, 180 * 16);
//              }
//           }
// 	  // skip timeconstant of update blocks and store neurons of the selected block
//       if (networkDialog->enet->net_block[i][0] == 1)
//        {
// 		 if (networkDialog->getcblock() == i)
// 		 {
// 		  selectedneurons[0] = networkDialog->enet->net_block[i][1];
// 		  selectedneurons[1] = networkDialog->enet->net_block[i][2];
// 		 }
//          for(t=networkDialog->enet->net_block[i][1]; t < (networkDialog->enet->net_block[i][1] + networkDialog->enet->net_block[i][2]); t++)
//            if (networkDialog->enet->neurontype[t] == 1)
//              p++;
//        }
//     }
// 	// draw neurons
//     for (i=0; i < networkDialog->enet->nneurons; i++)
//      {
//       if (networkDialog->enet->neuronbias[i] == 1)
//       {
//        w = networkDialog->biases[i];
// 	   if (w != networkDialog->enet->DEFAULT_VALUE)
// 	   {
//        if (w > networkDialog->enet->wrange)
//          w = networkDialog->enet->wrange;
//        if (w < (0.0 - networkDialog->enet->wrange))
//          w = 0.0 - networkDialog->enet->wrange;
//        cr = cg = cb = 255;
//        if (w > 0)
//          {
//            cg -= ((w / networkDialog->enet->wrange) * 255);
//            cb -= ((w / networkDialog->enet->wrange) * 255);
//          }
//          else
//          {
//            cg += ((w / networkDialog->enet->wrange) * 255);
//            cr += ((w / networkDialog->enet->wrange) * 255);
//          }
// 	   }
// 	   else
// 	   {
// 	     cr = 0;
// 		 cg = 185;
//          cb = 0;
// 	   }
//        wid = 2;
// 	   if (i >= selectedneurons[0] && i < selectedneurons[0]+selectedneurons[1])
// 		 wid = 4;
//       painter.setPen(QPen(QColor(cr,cg,cb,255), wid, Qt::SolidLine));
//        }
//       else
//        {
// 		wid = 1;
// 	    if (i >= selectedneurons[0] && i < selectedneurons[0]+selectedneurons[1])
// 		 wid = 4;
//         painter.setPen(QPen(Qt::black, wid, Qt::SolidLine));
//        }
//
//      if (networkDialog->enet->neurontype[i] == 0)
//          painter.setBrush(QBrush(QColor(75, 75, 75, 255), Qt::SolidPattern));		// standard logistic in dark-gray
//             else
//              if (networkDialog->enet->neurontype[i] == 1)
//                painter.setBrush(QBrush(QColor(255, 0, 0, 255), Qt::SolidPattern));		// dynamic with timeconstant in red
// 			  else
//                if (networkDialog->enet->neurontype[i] == 2)
//                  painter.setBrush(QBrush(QColor(0, 0, 255, 255), Qt::SolidPattern));		// binary in blue
// 			    else
//                   painter.setBrush(QBrush(QColor(150, 150, 150, 255), Qt::SolidPattern));	// logistic 20% in light-gray
//
//       painter.drawEllipse((float) (networkDialog->enet->neuronxy[i][0] - 5), (float) (networkDialog->enet->neuronxy[i][1] - 5), 10.0, 10.0);
//
//       // for neurons with gain parameters we display an additional white point in the centre of the circle
//       if (networkDialog->enet->neurongain[i] == 1)
//         {
//           painter.setPen(QPen(QColor(255,255,255,255), wid, Qt::SolidLine));
//           painter.setBrush(QBrush(QColor(255, 255, 255, 255), Qt::SolidPattern));
//           painter.drawEllipse((float) (networkDialog->enet->neuronxy[i][0] - 1), (float) (networkDialog->enet->neuronxy[i][1] - 1), 2.0, 2.0);
//         }
//
//       }
//
//     painter.setPen(Qt::black);
//     for (i=0; i < networkDialog->enet->nneurons; i++)
//       {
//         if (networkDialog->enet->neurondisplay[i] == 1)
//           painter.setPen(Qt::black);
//          else
//           painter.setPen(Qt::red);
//         if (i < networkDialog->ninputs)
//           labelxy.setRect((float) (networkDialog->enet->neuronxy[i][0] - 5), (float) (networkDialog->enet->neuronxy[i][1] + 5), 30, 30);
//          else
//           labelxy.setRect((float) (networkDialog->enet->neuronxy[i][0] - 5), (float) (networkDialog->enet->neuronxy[i][1] - 18), 30, 30);
//         painter.drawText(labelxy, networkDialog->neuroncl[i]);
//
// 		painter.setPen(Qt::darkRed);
// 		if (networkDialog->enet->neuronlesion[i]) painter.drawText(networkDialog->enet->neuronxy[i][0]-5,networkDialog->enet->neuronxy[i][1]-5,"X");//onofrio
//       }
//
//     painter.setBrush(Qt::black);
//     if (networkDialog->cneuronn == 1)
//        painter.drawEllipse((float) (networkDialog->enet->neuronxy[networkDialog->cneuron[0]][0] - 5), (float) (networkDialog->enet->neuronxy[networkDialog->cneuron[0]][1] - 5), 10.0, 10.0);
//     if (networkDialog->cneuronn >= 2)
//      {
//      if (networkDialog->cneuron[0] <= networkDialog->cneuron[1])
//       {
//        for (i=networkDialog->cneuron[0]; i <= networkDialog->cneuron[1]; i++)
//           painter.drawEllipse((float) (networkDialog->enet->neuronxy[i][0] - 5), (float) (networkDialog->enet->neuronxy[i][1] - 5), 10.0, 10.0);
//       }
//       else
//       {
//        painter.drawEllipse((float) (networkDialog->enet->neuronxy[networkDialog->cneuron[0]][0] - 5), (float) (networkDialog->enet->neuronxy[networkDialog->cneuron[0]][1] - 5), 10.0, 10.0);
//        painter.drawEllipse((float) (networkDialog->enet->neuronxy[networkDialog->cneuron[1]][0] - 5), (float) (networkDialog->enet->neuronxy[networkDialog->cneuron[1]][1] - 5), 10.0, 10.0);
//       }
//      }
//     painter.setBrush(Qt::blue);
//     if (networkDialog->cneuronn == 3)
//           painter.drawEllipse((float) (networkDialog->enet->neuronxy[networkDialog->cneuron[2]][0] - 5), (float) (networkDialog->enet->neuronxy[networkDialog->cneuron[2]][1] - 5), 10.0, 10.0);
//     if (networkDialog->cneuronn == 4 && networkDialog->cneuron[2] <= networkDialog->cneuron[3])
//       for (i=networkDialog->cneuron[2]; i <= networkDialog->cneuron[3]; i++)
//           painter.drawEllipse((float) (networkDialog->enet->neuronxy[i][0] - 5), (float) (networkDialog->enet->neuronxy[i][1] - 5), 10.0, 10.0);
//
//
//
// }
//
//
// /*
//  * handle mouse buttons
//  */
// void
// RendNetwork::mousePressEvent(QMouseEvent *event)
// {
//
//     int x,y,b;
//     int i,t;
//     int mode;
//     double cdist, dist;
//
//     x=event->x();
//     y=event->y();
//     b=event->button();
//
//
//     if (b == 2)
//     {
//       // right button has been pressed
//     }
//     else
//     {
//      // left button
//      if (b == 1)
//      {
//       mode = 0;
//       rnmousex = x;
//       rnmousey = y;
//
//       //   on a neuron: select the neuron (up to 4)
//       if (mode == 0)
//        for(i=0; i < networkDialog->enet->nneurons; i++)
//        {
//         if (mdist((float) x,(float) y, (float) networkDialog->enet->neuronxy[i][0] * networkDialog->cscalex, (float) networkDialog->enet->neuronxy[i][1] * networkDialog->cscaley) < 10.0)
//         {
//           if (networkDialog->cneuronn >= 4)
//              networkDialog->cneuronn = 0;
//            networkDialog->cneuron[networkDialog->cneuronn] = i;
//            networkDialog->cneuronn++;
//            mode = 2;
//            networkDialog->statusb->setText(QString("%1 neurons selected").arg(networkDialog->cneuronn));
//            emit selectedneuronsChanged();
//         }
//        }
//       //   on a connection: select the receiving and sending neuron
//       if (mode == 0)
//        {
//        dist = 999.99;
//        for (i=0; i < networkDialog->enet->net_nblocks; i++)
//         {
//          if (networkDialog->enet->net_block[i][0] == 0)
//            for (t=networkDialog->enet->net_block[i][1]; t < (networkDialog->enet->net_block[i][1] + networkDialog->enet->net_block[i][2]); t++)
//             for (b=networkDialog->enet->net_block[i][3]; b < (networkDialog->enet->net_block[i][3] + networkDialog->enet->net_block[i][4]); b++)
//              {
//               cdist = segmentdist((float) x, (float) y, (float) networkDialog->enet->neuronxy[t][0] * networkDialog->cscalex, (float) networkDialog->enet->neuronxy[t][1] * networkDialog->cscaley, (float) networkDialog->enet->neuronxy[b][0] * networkDialog->cscalex, (float) networkDialog->enet->neuronxy[b][1] * networkDialog->cscaley);
//               {
//                if (cdist < 1 && cdist < dist)
//                 {
//                  dist = cdist;
//                  networkDialog->cneuron[0] = t;
//                  networkDialog->cneuron[1] = t;
//                  networkDialog->cneuron[2] = b;
//                  networkDialog->cneuron[3] = b;
//                  networkDialog->cneuronn = 4;
//                  networkDialog->statusb->setText(QString("A single connection have been selected"));
//                  mode = 1;
//                 }
//               }
//              }
//          }
//         }
//       // otherwise remove selected neurons
//       // and reset the display modality to label
//       if (mode == 0)
//         {
//          networkDialog->cneuronn = 0;
//          networkDialog->pseudomode = 0;
//          networkDialog->pseudo_activate_net();
//          networkDialog->statusb->setText(QString("0 selected neurons"));
//          emit selectedneuronsChanged();
//         }
//       update();
//      }
//    }
//
//
//
// }
//
//
// /*
//  * handle mouse move events
//  */
// void
// RendNetwork::mouseMoveEvent(QMouseEvent *event)
// {
//
//     int x,y;
//     int i;
//     int mode;
//     QPoint xy;
//     int   cn;
//
//
//     x=event->x();
//     y=event->y();
//
//     mode = 0;
//
//     // move current neuron
//     networkDialog->cneuronn = 0;
//     if (mode == 0)
//       {
//        for(i=0, cn=-1; i < networkDialog->enet->nneurons; i++)
//          if (mdist((float) rnmousex,(float) rnmousey, (float) networkDialog->enet->neuronxy[i][0] * networkDialog->cscalex, (float) networkDialog->enet->neuronxy[i][1] * networkDialog->cscaley) < 10.0)
//            cn = i;
//        if (cn >= 0)
//           {
//             networkDialog->enet->neuronxy[cn][0] = (float) x / networkDialog->cscalex;
//             networkDialog->enet->neuronxy[cn][1] = (float) y / networkDialog->cscaley;
//             update();
//             rnmousex = x;
//             rnmousey = y;
//             mode = 1;
//             networkDialog->statusb->setText(QString("neuron position modified"));
//
//             //sprintf(sbuffer,"neuron #%d moved to %d %d", cn, enet->neuronxy[cn][0], enet->neuronxy[cn][1]);
//             //display_stat_message(sbuffer);
//           }
//       }
//
// }
//
//
// /*
//  * handle mouse release events by constraints movements to the grid (if any)
//  */
// void
// RendNetwork::mouseReleaseEvent(QMouseEvent *event)
// {
//
//     int i;
//     int mode;
//     QPoint xy;
//     float *o = nullptr;
//     float oldp1,oldp2;
//     int   cn;
//
//
//     if (networkDialog->grid > 0)
//     {
//
//     rnmousex=event->x();
//     rnmousey=event->y();
//
//     mode = 0;
//
//     // move current neuron
//     if (mode == 0)
//      for(i=0, cn=-1; i < networkDialog->enet->nneurons; i++)
//        if (mdist((float) rnmousex,(float) rnmousey, networkDialog->enet->neuronxy[i][0], networkDialog->enet->neuronxy[i][1]) < 10.0)
//          cn = i;
//      if (cn >= 0)
//        {
//         oldp1 = networkDialog->enet->neuronxy[cn][0];
//         oldp2 = networkDialog->enet->neuronxy[cn][1];
//         networkDialog->enet->neuronxy[cn][0] = (float) (((int) networkDialog->enet->neuronxy[cn][0]) / networkDialog->grid * networkDialog->grid);
//         networkDialog->enet->neuronxy[cn][1] = (float) (((int) networkDialog->enet->neuronxy[cn][1]) / networkDialog->grid * networkDialog->grid);
//         if ((oldp1 - networkDialog->enet->neuronxy[cn][0]) > (networkDialog->grid / 2))
//           networkDialog->enet->neuronxy[cn][0] += networkDialog->grid;
//         if ((oldp2 - networkDialog->enet->neuronxy[cn][1]) > (networkDialog->grid / 2))
//           networkDialog->enet->neuronxy[cn][1] += networkDialog->grid;
//         update();
//         networkDialog->statusb->setText(QString("neuron's position modified"));
//         //sprintf(sbuffer,"neuron #%d released to %d %d", cn, enet->neuronxy[cn][0], enet->neuronxy[cn][1]);
//         //display_stat_message(sbuffer);
//         mode = 1;
//        }
//
//     if (mode == 3)
//         {
//         oldp1 = *o;
//         oldp2 = *(o + 1);
//         *o = (float) (((int) *o) / networkDialog->grid * networkDialog->grid);
//         *(o + 1) = (float) (((int) *(o + 1)) / networkDialog->grid * networkDialog->grid);
//         if ((oldp1 - *o) > (networkDialog->grid / 2))
//           *o += networkDialog->grid;
//         if ((oldp2 - *(o + 1)) > (networkDialog->grid / 2))
//           *(o + 1) += networkDialog->grid;
//         networkDialog->statusb->setText(QString("neuron's position modified"));
//         //sprintf(sbuffer,"object %s #%d released to %.1f %.1f", nobject, idobject, *o, *(o + 1));
//         //display_stat_message(sbuffer);
//         update();
//         }
//
//     }
//
//
// }
//
// /*
//  * handle mouse buttons
//  */
// void
// RendNetwork::mouseDoubleClickEvent(QMouseEvent* /*event*/)
// {
//
// }
//
//
//
// double
// RendNetwork::mdist(float x, float y, float x1, float y1)
//
// {
//    double qdist;
//
//    qdist = ((x-x1)*(x-x1)) + ((y-y1)*(y-y1));
//    return(sqrt(qdist));
// }
//
// /*
// * estimate the distance between a point (p) and a segment (a-b)
// * by computing dist(p,a) + dist(p,b) - dist(a,b)
// * if one of the two distances are greater than the segment
// * assume that the point is too far or is outside the segment
// */
// double
// RendNetwork::segmentdist(float px,float py,float ax,float ay,float bx,float by)
// {
//   double dab,dpa,dpb;
//
//   dab = sqrt(((ax-bx)*(ax-bx)) + ((ay-by)*(ay-by)));
//   dpa = sqrt(((px-ax)*(px-ax)) + ((py-ay)*(py-ay)));
//   dpb = sqrt(((px-bx)*(px-bx)) + ((py-by)*(py-by)));
//
//   if (dpa < dab && dpb < dab)
//     return((dpa+dpb)-dab);
//   else
//     return(99999.999);
// }
//
// /* EvoSlider */
// EvoSlider::EvoSlider(float *ref, float rangemin , float rangemax, QWidget * parent)
// : QDoubleSpinBox(parent) {
// 	vref=ref;
// 	setRange(rangemin, rangemax);
// 	setSingleStep(0.05);
// 	setValue(*vref);
// 	connect(this,SIGNAL(valueChanged(double)),this, SLOT(updateValue(double)));
// }
//
// void EvoSlider::updateValue(double ival)
// {
// 	*vref=(float)ival;
// 	emit networkChanged();
// }
//
// /* Mixer */
// MixerDialog::MixerDialog(Evonet *enet, NetworkDialog* netd)
// : QWidget( netd ) {
// 	evonet=enet;
//     netdl=netd;
// 	trialRef=0.0;
// 	layout=new QGridLayout(this);
// }
//
// void MixerDialog::setUpMixer()
// {
// 	// We don't use findChildren<QWidget*> because it finds children recursively and
// 	// could lead to double-frees. The dynamic_cast ensures that only QWidget children
// 	// are deleted
// 	foreach(QObject* child, children()) {
// 		delete dynamic_cast<QWidget*>(child);
// 	}
//
// 	if(evonet->neuronlesions || evonet->nselected > 0)
// 		{
// 		    statusmixer = new QLabel("",this);
//             if (netdl->pseudomode == 2)
// 				statusmixer->setText(QString("weights:"));
//             if (netdl->pseudomode == 1)
// 				statusmixer->setText(QString("t-const:"));
//             if (netdl->pseudomode == 3)
// 				statusmixer->setText(QString("biases:"));
//             if (netdl->pseudomode == 4)
// 				statusmixer->setText(QString("gains:"));
// 			if  (evonet->neuronlesions)
// 				statusmixer->setText(QString("neurons:"));
// 		    layout->addWidget(statusmixer,0,0);
// 			Logger::info(QString("Mixer for n.%1 selected parameters\n").arg(evonet->neuronlesions+evonet->nselected));
// 		}
//
// 	float **gp;
//     float *v;
// 	int l;
// 	int lt,li;
// 	int line=0;
//
// 	// selected parameters
//     if (evonet->nselected > 0)
// 	{
//      gp = evonet->selectedp;
//      v = *gp;
// 	 if (netdl->pseudomode == 2)
// 	 {
// 	   // weights
// 	   lt = netdl->cneuron[0];
// 	   li = netdl->cneuron[2];
// 	   line = 0;
// 	   for(int i=0;i<evonet->nselected;i++,v++,li++)
// 		{
// 		  if (li > netdl->cneuron[3])
// 		   {
// 		     lt++;
// 			 li = netdl->cneuron[2];
// 			 line++;
// 		   }
// 		  layout->addWidget(new QLabel(evonet->neuronl[lt],this),1,i);
// 		  EvoSlider* slide = new EvoSlider(v,(0 - evonet->getWrange()),evonet->getWrange(),this);
// 		  connect( slide, SIGNAL(networkChanged()), netdl->rendNetwork, SLOT(update()) );
// 		  layout->addWidget(slide,2,i);
// 		  layout->addWidget(new QLabel(evonet->neuronl[li],this),3,i);
// 		}
// 	  }
// 	 else
// 	 {
// 	   // timeconstant(1), bias (2), gain(3)
// 	   l = netdl->cneuron[0];
// 	   for(int i=0;i<evonet->nselected;i++,v++,l++)
// 		{
// 		  layout->addWidget(new QLabel(evonet->neuronl[l],this),1,i);
// 		  EvoSlider* slide = new EvoSlider(v,(0 - evonet->getWrange()),evonet->getWrange(),this);
// 		  connect( slide, SIGNAL(networkChanged()), netdl->rendNetwork, SLOT(update()) );
// 		  layout->addWidget(slide,2,i);
// 	    }
// 	 }
// 	}
// 	else
// 	{
// 	// state of lesioned neurons
// 	for(int i=0;i<evonet->getNoNeurons();i++)
// 		if(evonet->neuronlesion[i])
// 		{
// 			layout->addWidget(new QLabel(evonet->neuronl[i],this),1,i);
// 		    EvoSlider* slide = new EvoSlider(&evonet->neuronlesionVal[i],evonet->neuronrange[i][0],evonet->neuronrange[i][1],this);
// 		    connect( slide, SIGNAL(networkChanged()), netdl->rendNetwork, SLOT(update()) );
// 		    layout->addWidget(slide,2,i);
// 		}
// 	}
//
// }
//
// void MixerDialog::setNet(Evonet* n)
// {
// 	evonet = n;
// }
//
// } //end namespace salsa
//
// // All the suff below is to restore the warning state on Windows
// #if defined(_MSC_VER)
// 	#pragma warning(pop)
// #endif
