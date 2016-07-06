// /********************************************************************************
//  *  FARSA Experiments Library                                                   *
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
// #ifndef RENDNETWORK_H
// #define RENDNETWORK_H
//
// #include <QBrush>     // for rendnetwork
// #include <QPen>
// #include <QPixmap>
// #include <QWidget>
// #include <QShortcut>
//
// #include <QWidget>    // for NetworkDialog
// #include <QGridLayout>
// #include <QToolBar>
// #include <QBoxLayout>
// #include <QSlider>
// #include <QDoubleSpinBox>
// #include <QLabel>
// #include <QComboBox>
// #include "evonet.h"
//
// namespace farsa {
//
// class MixerDialog;
// class RendNetwork;
//
// /*
//  * The dialog used to show the network widget and the associated menu bar
//  */
// class FARSA_EXPERIMENTS_API NetworkDialog : public QWidget
// {
// 	Q_OBJECT
//
// public:
//     // the network rendering widget
//     RendNetwork *rendNetwork;
// 	// the mixer widget that allow the user to change parameters values
// 	MixerDialog *mixerDialog;
// 	// the network
// 	Evonet  *enet;
// 	// status bar of the dialog
//     QLabel  *statusb;
// 	// user's selected neurons
// 	int     cneuron[4];
// 	// n. of user's selected cneurons
// 	int     cneuronn;
// 	// the current cycle
// 	int     rnrun;
// 	// mouse left-clicked x position
// 	int     rnmousex;
// 	// mouse left-clicked y position
// 	int     rnmousey;
// 	// vector of parameter displayed through the graphic interface (max length = nneurons)
// 	float   **graphicp;
// 	// number of currently extracted parameters
// 	int     ngp;
// 	// x scale for the controller rendering
// 	float   cscalex;
// 	// y scale for the controller rendering
// 	float   cscaley;
// 	// the display modality (0=label 1=delta, 2=weight, 3=biases, 4=gain)
// 	int     pseudomode;
// 	// return the current selected block
// 	int     getcblock();
// 	// extract labels and/or values to be displayed/modified
// 	void    pseudo_activate_net();
// 	// pointer to free parameters
// 	float	*freep;
// 	// number of free parameters
// 	int nfreep;
// 	// n. input units
//     int ninputs;
// 	// n. hidden units
//     int nhiddens;
// 	// n. output units
//     int noutputs;
// 	// total n. of neurons
//     int nneurons;
// 	// neurons current labels (updated by pseudo_activate_net())
//     char neuroncl[Evonet::MAXN][8];
// 	// the step of the grid (0 = no grid)
// 	int  grid;
// 	// whether some of the neurons has been lesioned or manually set
//     int neuronlesions;
// 	// biases value
// 	float biases[Evonet::MAXN];
// 	// display an error message
// 	void error(const char *emessage);
// 	// display a warning message
// 	void warning(const char *emessage);
//
// 	// Constructor
// 	NetworkDialog(Evonet* net, QWidget* parent=0, Qt::WindowFlags flags=0);
//
// 	// destructor
// 	virtual ~NetworkDialog();
//
// 	void setNet(Evonet* n);
//
// signals:
// 	void selectedblockChanged();	    // emitted when the selected block is modified
//
// private:
//
//
// 	/**
// 	 * \brief The widget shown by this dialog
// 	 */
// 	QWidget *m_widget;
//
// 	/**
// 	 * \brief The main layout of the dialog. Contains the toolbar (if
// 	 *        present) and m_layout
// 	 */
// 	QBoxLayout *m_mainLayout;
//
// 	/**
// 	 * \brief The toolbar
// 	 *
// 	 * This is NULL until the first call to toolBar()
// 	 */
// 	QToolBar *m_toolBar;
//
// 	// create the toolbar
// 	void createToolBars();
// 	// creates actions associated to menu commands
// 	void createActions();
// 	// generate a string containing the name of a block
// 	void block_name(int i, char *st);
//     // set the neurons to be displayed in the neurons' monitor
// 	QAction *set_neurondisplayAct;
// 	// set whether neurons have gains
// 	QAction *set_neurongainAct;
// 	//set whether neurons have biases
// 	QAction *set_neuronbiasAct;
// 	//set whether neurons are lesioned
// 	QAction *set_lesionAct;
// 	//display neurons labels
// 	QAction *display_labelAct;
// 	//display and allow to modify weights
// 	QAction *display_weightAct;
// 	//display and allow to modify time constants
// 	QAction *display_deltaAct;
// 	//display and allow to modify biases
// 	QAction *display_biasAct;
// 	//display and allow to modify gain
// 	QAction *display_gainAct;
// 	//erase blocks
// 	QAction *erase_Act;
// 	//add an update block
// 	QAction *add_ublockAct;
// 	//add a connection block
// 	QAction *add_cblockAct;
// 	//add a gain block
// 	QAction *add_gblockAct;
// 	//increase the value of selected parameters
// 	QShortcut *increasevAct;
// 	//decrease the value of selected parameters
// 	QShortcut *decreasevAct;
// 	//open a file
// 	QAction *openAct;
// 	//save a file
// 	QAction *saveAct;
// 	//modify the neuron type
//     QComboBox *neurontypeAct;
//     //select a block
// 	QComboBox *cblockAct;
// 	//change the block type
//     QComboBox *blocktypeAct;
//
// 	private slots:
// 	// update the combo box of the neuron type
// 	void updatetoolb();
// 	// update the combo box of the block type
// 	void updateblocktype();
// 	//change the type of a block
// 	void changeblocktype(int t);
// 	// set whether neurons have to be displayed by the monitor
// 	void set_neurondisplay();
// 	// set whether neurons have biases
// 	void set_neuronbias();
// 	// set the neuron type
// 	void set_neurontype(int i);
// 	// update the combo box when the selected block has been modified
//     void selblock_changed(int i);
// 	// set whether neurons have gains
// 	void set_neurongain();
// 	// lesion/unlesion neurons
// 	void set_lesion();
// 	// display neurons labels
// 	void display_label();
// 	// display/modify weights
// 	void display_weight();
// 	// display/modify timeconstants
// 	void display_delta();
// 	//display/modify biases
// 	void display_bias();
// 	//display/modify gains
// 	void display_gain();
// 	//erase blocks
// 	void erase();
// 	//add an update block
// 	void add_ublock();
// 	//add a connection block
// 	void add_cblock();
// 	//add a gain block
// 	void add_gblock();
// 	//increase the value of selected parameters
// 	void increasev();
// 	//decrease the value of selected parameters
// 	void decreasev();
// 	//open a file
// 	void open();
// 	//save a file
// 	void save();
// 	void openMixer();
//
// };
//
//
// /*
//  * The network widget use to show the network architecture and parameters
//  */
// class FARSA_EXPERIMENTS_API RendNetwork : public QWidget
// {
//     Q_OBJECT
//
// public:
//     enum Shape { Line, Points, Polyline, Polygon, Rect, RoundRect, Ellipse, Arc,
//                  Chord, Pie, Path, Text, Pixmap };
//
//     RendNetwork( NetworkDialog* networkDialog, QWidget *parent = 0);
//
//     QSize minimumSizeHint() const;
//     QSize sizeHint() const;
// 	// mouse left-clicked x position
//     int     rnmousex;
// 	// mouse left-clicked y position
//     int     rnmousey;
// 	// distance between two points
//     double mdist(float x, float y, float x1, float y1);
// 	//distance beween a point and a segment
//     double segmentdist(float px,float py,float ax,float ay,float bx,float by);
//
// signals:
// 	//signal when the first selected neuron has been modified
// 	void selectedneuronsChanged();
//
// public slots:
//     void setShape(Shape shape);
//     void setPen(const QPen &pen);
//     void setBrush(const QBrush &brush);
//     void setAntialiased(bool antialiased);
//     void setTransformed(bool transformed);
//
// protected:
// 	// display the network (neurons and connection weights
//     void paintEvent(QPaintEvent *event);
//     void mousePressEvent(QMouseEvent *event);
//     void mouseMoveEvent(QMouseEvent *event);
//     void mouseReleaseEvent(QMouseEvent *event);
//     void mouseDoubleClickEvent(QMouseEvent *event);
//
// private:
//     Shape shape;
//     QPen pen;
//     QBrush brush;
//     bool antialiased;
//     bool transformed;
//     QPixmap pixmap;
// 	NetworkDialog* networkDialog;
// };
//
// /*
//  * Slider use to visualize/modify parameters
//  */
// class FARSA_EXPERIMENTS_API EvoSlider : public QDoubleSpinBox
// {
// 	Q_OBJECT
// public:
// 	// slider creator
// 	EvoSlider(float *ref, float rangemin=0, float rangemax=1, QWidget *parent=0);
// signals:
// 	void networkChanged();
// private:
// 	// slider value
// 	float *vref;
//     RendNetwork *rendnet;
// private slots:
// 	void updateValue(double ival);
//
// };
//
// /*
//  * Mixer containing sliders used to visualize/modify selected parameters of the neural network
//  */
// class FARSA_EXPERIMENTS_API MixerDialog : public QWidget
// {
// 	Q_OBJECT
//
// public:
// 	MixerDialog(Evonet *enet, NetworkDialog* netd);
// 	void setUpMixer();
// 	void setNet(Evonet* n);
// private:
// 	// the network
// 	Evonet *evonet;
// 	// the dialog box containing the network widget and the associated menu bar
//     NetworkDialog* netdl;
// 	// the Layout of the mixer widget
// 	QGridLayout* layout;
// 	// the status bar of the mixer
//     QLabel  *statusmixer;          // status bar of the mixer widget
// 	double trialRef;
// };
//
// } //end namespace farsa
//
// #endif
