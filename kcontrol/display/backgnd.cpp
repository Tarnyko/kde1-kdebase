
//
// KDE Display background setup module
//
// Copyright (c)  Martin R. Jones 1996
//
// Gradient fill added by Mark Donohoe 1997
//
// Converted to a kcc module by Matthias Hoelzer 1997
//

#ifdef HAVE_CONFIG
#include <config.h>
#endif

#include <qgrpbox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qfiledlg.h>
#include <qradiobt.h>
#include <qchkbox.h>
#include <qpainter.h>
#include <qlayout.h>
#include <kapp.h>
#include <kwm.h>
#include <stdlib.h>

#include <X11/Xlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "backgnd.h"
#include "dither.h"
#include "kpixmap.h"

#include "backgnd.moc"

#include <kiconloader.h>
#include <kwm.h>
#include <ksimpleconfig.h>
#include <kbuttonbox.h>

#ifdef HAVE_LIBJPEG
#include "jpeg.h"
#endif

#define NO_WALLPAPER	i18n("(none)")

//----------------------------------------------------------------------------

class GPixmap : public QPixmap
{
public:
    GPixmap();
    void gradientFill(QColor color1, QColor color2, bool updown = true, 
		      int num_colors = 8);
    void patternFill( QColor color1, QColor color2, uint pattern[8] );
    void flatFill ( QColor color);

private:
    QPixmap cropped_pm;
    QPixmap full_pm;
    int x_size;
    int y_size;
    QColor col;
    int h, s;
    int red1, green1, blue1;
    int red_dif, green_dif, blue_dif;
    float rat;
    uint *p;
    uint rgbcol;
    int steps;
};

GPixmap::GPixmap()
{}

void GPixmap::gradientFill(QColor color1, QColor color2, 
			   bool updown, int num_colors)
{    
    if(updown)
    	y_size=height();
    else
    	y_size=width();
    
    cropped_pm.resize( 30, y_size );
    
    QImage image(30, y_size, 32);
    
    
    red1 = color1.red();
    green1 = color1.green();
    blue1 = color1.blue();
    
    red_dif = color2.red()-color1.red();
    green_dif = color2.green()-color1.green();
    blue_dif = color2.blue()-color1.blue();
    
    for ( s = y_size-1; s > 0; s-- )
	{
	    p = (uint *) image.scanLine( y_size - s - 1 );
	    rat=1.0*s/y_size;
	    col.setRgb( red1+(int)(red_dif*rat),
			green1+(int)(green_dif*rat), blue1+(int)(blue_dif*rat) );
	    rgbcol= col.rgb();
	    
	    for( h = 0; h < 30; h++ ) {
		*p = rgbcol;
		p++;
	    }
	}
	
	if(num_colors<2 || num_colors >256) {
		num_colors=8;
	}
	
	QColor *ditherPalette = new QColor[num_colors];
	for( s=0; s<num_colors; s++) {
		ditherPalette[s].setRgb(red1+red_dif*s/(num_colors-1),
		green1+green_dif*s/(num_colors-1),
		blue1+blue_dif*s/(num_colors-1) );
	}
	
	kFSDither dither( ditherPalette, num_colors );
	QImage tImage = dither.dither( image );
	cropped_pm.convertFromImage( tImage );
	
	if(updown)
	    steps=width()/20+1;
	else
	    steps=height()/20+1;
	
	QPainter p;
	p.begin( this );
	if(updown)	
	    for(s=0;s<steps;s++)
		p.drawPixmap(20*s, 0,cropped_pm, 5, 0 , 20, y_size );
	else {
	    QWMatrix matrix;
	    matrix.translate( (float)width()-1.0, 0.0);
	    matrix.rotate( 90.0 );
	    p.setWorldMatrix( matrix );
	    for(s=0;s<steps;s++)
		p.drawPixmap(20*s, 0,cropped_pm, 5, 0 , 20, y_size );
	}
	p.end();
	
	delete [] ditherPalette;
}	 

void GPixmap::patternFill( QColor color1, QColor color2, uint pattern[8] )
{
    QPixmap tile(8, 8);
    tile.fill(color2);
    QPainter pt;
    pt.begin(&tile);
    pt.setBackgroundColor( color2 );
    pt.setPen( color1 );
    
    for (int y = 0; y < 8; y++) {
	uint v = pattern[y];
	for (int x = 0; x < 8; x++) {
	    if ( v & 1 )
		pt.drawPoint(7 - x, y);
	    v /= 2;
	}
    }
    
    pt.end();

    int sx, sy = 0;
    while (sy < height()) {
	sx = 0;
	while (sx < width()) {
	    bitBlt( this, sx, sy, &tile, 0, 0, 8, 8);
	    sx += 8;
	}
	sy += 8;
    }
}

void GPixmap::flatFill( QColor color )
{
    fill( color );
}
//----------------------------------------------------------------------------

KRenameDeskDlg::KRenameDeskDlg( const char *t, QWidget *parent )
    : QDialog( parent, 0, true )
{
    
    QVBoxLayout *vlayout = new QVBoxLayout( this, 5 );

	vlayout->addSpacing(10);

	QLabel *label = new QLabel( this );
	label->setText( i18n( "Enter new desktop name" ) );
	label->setMinimumSize( label->sizeHint() );
	vlayout->addWidget( label );

    edit = new QLineEdit( this );
    edit->setText( t );
    edit->setFixedHeight( edit->sizeHint().height() );
    edit->setFocus();
    vlayout->addWidget( edit );
	
	vlayout->addStretch(10);
	vlayout->addSpacing(10);
	
	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( this );
	tmpQFrame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	tmpQFrame->setMinimumHeight( tmpQFrame->sizeHint().height() );
	
	vlayout->addWidget( tmpQFrame );
	
	KButtonBox *bbox = new KButtonBox( this );
	bbox->addStretch( 10 );
	
	QPushButton *ok = bbox->addButton( i18n( "OK" ) );
	connect( ok, SIGNAL( clicked() ), SLOT( accept() ) );
	
	QPushButton *cancel = bbox->addButton( i18n( "Cancel" ) );
	connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );
	
	bbox->layout();
	vlayout->addWidget( bbox );

    vlayout->activate();
	
	resize(150,0);
}

//----------------------------------------------------------------------------

KBackground::KBackground( QWidget *parent, int mode, int desktop )
	: KDisplayModule( parent, mode, desktop )
{
#ifdef HAVE_LIBJPEG
    QImageIO::defineIOHandler("JFIF","^\377\330\377\340", 
			  0, read_jpeg_jfif, NULL);
#endif

    KIconLoader iconLoader;

    changed = false;
    maxDesks = 8;
    deskNum = 0;

    setName( i18n("Desktop") );

    // if we are just initialising we don't need to create setup widget
    if ( mode == Init )
    {
//	readSettings( 0 );
	return;
    }

    if ( KWM::isKWMInitialized() )
    {
	maxDesks = KWM::numberOfDesktops();
	deskNum = KWM::currentDesktop() - 1;
    }
    else
	maxDesks = 1;

    readSettings( deskNum );

    QPushButton *button;
    QGroupBox *group;
    QRadioButton *rb;
	
	QGridLayout *topLayout = new QGridLayout( this, 4, 4, 10 );
	
	topLayout->setRowStretch(0,0);
	topLayout->setRowStretch(1,10);
	topLayout->setRowStretch(2,100);
	topLayout->setRowStretch(3,0);
	
	topLayout->setColStretch(0,0);
	topLayout->setColStretch(1,100);
	topLayout->setColStretch(2,10);
	topLayout->setColStretch(3,0);
	
	group = new QGroupBox( i18n("Desktop"), this );

	topLayout->addWidget( group, 1,1 );
	
	QBoxLayout *groupLayout = new QVBoxLayout( group, 10 );

    deskListBox = new QListBox( group );
    getDeskNameList();
    deskListBox->setCurrentItem( deskNum );
    connect( deskListBox, SIGNAL( highlighted(int) ),
							SLOT( slotSwitchDesk(int) ) );
	
	groupLayout->addSpacing( 20 );
	groupLayout->addWidget( deskListBox, 10 );
	groupLayout->addSpacing( 0 );
	
    button = new QPushButton( i18n( "&Rename ..." ), group );
	button->setFixedHeight( button->sizeHint().height() );
    if ( !KWM::isKWMInitialized() )
	button->setEnabled( false );
    connect( button, SIGNAL( clicked() ), SLOT( slotRenameDesk() ) );
	
	groupLayout->addSpacing( 0 );
	groupLayout->addWidget( button, 10 );
	groupLayout->addSpacing( 0 );

    QPixmap p = iconLoader.loadIcon("monitor.xpm");

    monitorLabel = new QLabel( this );
	monitorLabel->setAlignment( AlignCenter );
    monitorLabel->setPixmap( p );
    monitorLabel->setMinimumSize( 220, 160 );
	
	topLayout->addWidget( monitorLabel, 1, 2 );

    monitor = new KBGMonitor( monitorLabel );
    monitor->setBackgroundColor( color1 );
	
    group = new QGroupBox( i18n( "Colors" ), this );
	
	QGridLayout *grid = new QGridLayout( group, 8, 5, 5 );
	
	grid->setRowStretch(0,10);
	grid->setRowStretch(1,10);
	grid->setRowStretch(2,10);
	grid->setRowStretch(3,10);
	grid->setRowStretch(4,10);
	grid->setRowStretch(5,10);
	grid->setRowStretch(6,10);
	grid->setRowStretch(7,0);

	grid->setColStretch(0,0);
	grid->setColStretch(1,10);
	grid->setColStretch(2,100);
	grid->setColStretch(3,100);
	grid->setColStretch(4,0);
	
	ncGroup = new QButtonGroup( this );
	ncGroup->hide();
    ncGroup->setExclusive( true );

    rb = new QRadioButton( i18n("&One Color"), group );
	rb->setFixedHeight( rb->sizeHint().height() );
    ncGroup->insert( rb, OneColor );
	
	grid->addMultiCellWidget( rb, 1, 1, 1, 3 );

    colButton1 = new KColorButton( group );
    connect( colButton1, SIGNAL( changed( const QColor & ) ),
	    SLOT( slotSelectColor1( const QColor & ) ) );
		
	grid->addMultiCellWidget( colButton1, 2, 2, 2, 3 );

 	rb = new QRadioButton( i18n("&Two Color"), group );
	rb->setFixedHeight( rb->sizeHint().height() );
    ncGroup->insert( rb, TwoColor );
    
	grid->addMultiCellWidget( rb, 3, 3, 1, 3 );
    
    connect( ncGroup, SIGNAL( clicked( int ) ), SLOT( slotColorMode( int ) ) );

    colButton2 = new KColorButton( group );
    connect( colButton2, SIGNAL( changed( const QColor & ) ),
	    SLOT( slotSelectColor2( const QColor & ) ) );
			
	grid->addMultiCellWidget( colButton2, 4, 4, 2, 3 );
	
    stGroup = new QButtonGroup( this );
    stGroup->hide();
    stGroup->setExclusive( true );

    rbGradient = new QRadioButton( i18n( "Blen&d" ), group );
	rbGradient->setFixedHeight( rb->sizeHint().height() );
    stGroup->insert( rbGradient, Gradient );
	
	grid->addWidget( rbGradient, 5, 2 );
	
    rbPattern = new QRadioButton( i18n( "&Pattern" ), group );
	rbPattern->setFixedHeight( rb->sizeHint().height() );
    stGroup->insert( rbPattern, Pattern );

	grid->addWidget( rbPattern, 5, 3 );
	
    connect( stGroup, SIGNAL( clicked( int ) ), SLOT( slotStyleMode( int ) ) );

    changeButton = new QPushButton( i18n("Set&up ..."), group );
	changeButton->setFixedHeight( changeButton->sizeHint().height() );
	colButton1->setFixedHeight( changeButton->sizeHint().height() );
	colButton2->setFixedHeight( changeButton->sizeHint().height() );
    connect(changeButton, SIGNAL(clicked()) , SLOT(slotSetup2Color()) );
	
	grid->addMultiCellWidget( changeButton, 6, 6, 2, 3 );

	topLayout->addWidget( group, 2, 1 );

    group = new QGroupBox( i18n("Wallpaper"), this );

	groupLayout = new QVBoxLayout( group, 10 ); 
    groupLayout->addSpacing( 10 );
	groupLayout->addStretch( 5 );

    wpGroup = new QButtonGroup( this );
    wpGroup->hide();
    wpGroup->setExclusive( true );

    rb = new QRadioButton( i18n("Ti&led"), group );
	rb->setFixedHeight( rb->sizeHint().height() );
    wpGroup->insert( rb, Tiled );
	
	groupLayout->addWidget( rb, 10 );
	
    rb = new QRadioButton( i18n("&Centred"), group );
	rb->setFixedHeight( rb->sizeHint().height() );
    wpGroup->insert( rb, Centred );
	
	groupLayout->addWidget( rb, 10 );
	
    rb = new QRadioButton( i18n("&Scaled"), group );
	rb->setFixedHeight( rb->sizeHint().height() );
    wpGroup->insert( rb, Scaled );
	
	groupLayout->addWidget( rb, 10 );
	
    connect( wpGroup, SIGNAL( clicked( int ) ), SLOT( slotWallpaperMode( int ) ) );

	QString path = kapp->kde_wallpaperdir().copy();
    QDir d( path, "*", QDir::Name, QDir::Readable | QDir::Files );
    const QStrList *list = d.entryList();

    wpCombo = new QComboBox( group );
    wpCombo->insertItem( NO_WALLPAPER, 0 );
    wpCombo->setCurrentItem( 0 );
	wpCombo->setFixedHeight( wpCombo->sizeHint().height() );
	
	groupLayout->addStretch( 5 );
	groupLayout->addWidget( wpCombo, 10 );
	
    QStrListIterator it( *list );
    for ( int i = 1; it.current(); ++it, i++ )
    {
	wpCombo->insertItem( it.current() );
	if ( wallpaper == it.current() )
	    wpCombo->setCurrentItem( i );
    }

    if ( wallpaper != NO_WALLPAPER && wpCombo->currentItem() == 0 )
    {
	wpCombo->insertItem( wallpaper );
	wpCombo->setCurrentItem( wpCombo->count()-1 );
    }
    connect( wpCombo, SIGNAL( activated( const char * ) ),
		SLOT( slotWallpaper( const char * )  )  );
		
    button = new QPushButton( i18n("&Browse..."), group );
	button->setFixedHeight( button->sizeHint().height() );
    connect( button, SIGNAL( clicked() ), SLOT( slotBrowse() ) );

	groupLayout->addWidget( button, 10 );

	topLayout->addWidget( group, 2, 2 );
	topLayout->activate();
    
    showSettings();
}

void KBackground::resizeEvent( QResizeEvent * )
{
	monitor->setGeometry( (monitorLabel->width()-200)/2+20,
		(monitorLabel->height()-160)/2+10, 157, 111 );
}

void KBackground::readSettings( int num )
{
    char group[80];
    sprintf( group, "Desktop%d", num + 1 );

    KConfig *config = kapp->getConfig();
    config->setGroup( group );
    
    QString str;
    
    str = config->readEntry( "Color1" );
    if ( !str.isNull() )
	color1.setNamedColor( str );
    else
	color1 = gray;
    
    str = config->readEntry( "Color2" );
    if ( !str.isNull() )
	color2.setNamedColor( str );
    else
	color2 = gray;
    
    ncMode=OneColor;
	stMode=Gradient;
    str = config->readEntry( "ColorMode" );
    if ( !str.isNull() )
	{
	    if ( str == "Gradient" ) {
			ncMode = TwoColor;
			stMode = Gradient;
	    } else if (str == "Pattern") {
			ncMode = TwoColor;
			stMode = Pattern;
		}
	}

    QStrList strl;
    config->readListEntry("Pattern", strl);
    uint size = strl.count();
    if (size > 8) size = 8;
    uint i = 0;
    for (i = 0; i < 8; i++)
	pattern[i] = (i < size) ? QString(strl.at(i)).toUInt() : 255;
    
    orMode=Portrait;
    str = config->readEntry( "OrientationMode" );
    if ( !str.isNull() )
	{
	    if ( str == "Landscape" )
		orMode = Landscape;
	}
    
    wpMode = Tiled;
    str = config->readEntry( "WallpaperMode" );
    if ( !str.isNull() )
	{
	    if ( str == "Centred" )
		wpMode = Centred;
	    else if ( str == "Scaled" )
		wpMode = Scaled;
	}
    
    wallpaper = config->readEntry( "Wallpaper", NO_WALLPAPER );
    loadWallpaper( wallpaper );
}

void KBackground::writeSettings( int num )
{
        if ( !changed ) {
			return;
		}

	char group[80];
	sprintf( group, "Desktop%d", num + 1 );
	
	KConfig *config = kapp->getConfig();
	config->setGroup( group );
	
	QString col1Name(10);
	col1Name.sprintf("#%02x%02x%02x", color1.red(), color1.green(), color1.blue());
	config->writeEntry( "Color1", col1Name );
	
	QString col2Name(10);
	col2Name.sprintf("#%02x%02x%02x", color2.red(), color2.green(), color2.blue());
	config->writeEntry( "Color2", col2Name );

	config->writeEntry( "Wallpaper", wallpaper );

	switch ( wpMode )
	{
		case Tiled:
			config->writeEntry( "WallpaperMode", "Tiled" );
			break;
		case Centred:
			config->writeEntry( "WallpaperMode", "Centred" );
			break;
		case Scaled:
			config->writeEntry( "WallpaperMode", "Scaled" );
			break;
	}
	
	if ( ncMode == OneColor ) {
		config->writeEntry( "ColorMode", "Flat" );
	} else if ( ncMode == TwoColor && stMode == Gradient ) {
		config->writeEntry( "ColorMode", "Gradient" );		
	} else if ( ncMode == TwoColor && stMode == Pattern ) {
		config->writeEntry( "ColorMode", "Pattern" );
	} else printf("ColorMode not changed\n");
	
	QStrList strl( true ); // deep copies
	for (uint i = 0; i < 8 ; i++) {
	    char buffer[10];
	    sprintf(buffer, "%d",pattern[i]);
	    strl.append(buffer);
	}
	config->writeEntry( "Pattern", strl);

	switch ( orMode )
	{
		case Portrait:
			config->writeEntry( "OrientationMode", "Portrait" );
			break;
		case Landscape:
			config->writeEntry( "OrientationMode", "Landscape" );
			break;
	}

	changed = false;

	config->sync();
}

void KBackground::getDeskNameList()
{
    int i;
    int current = deskListBox->currentItem();

    if ( current < 0 )
	current = 0;

    deskListBox->setUpdatesEnabled( false );
    deskListBox->clear();

    if ( KWM::isKWMInitialized() )
    {
	for ( i = 0; i < maxDesks; i++ )
	    deskListBox->insertItem( KWM::getDesktopName(i+1) );
    }
    else
       deskListBox->insertItem( i18n( "Default" ) );
    
    deskListBox->setCurrentItem( current );
    deskListBox->setUpdatesEnabled( true );
}

void KBackground::setDesktop( int desk )
{
    if ( deskNum == desk )
	return;

    writeSettings( deskNum );

    deskNum = desk;

    if ( deskNum <= 0 )
	deskNum = 0;

    if ( deskNum >= maxDesks - 1 )
	deskNum = maxDesks - 1;

    readSettings( deskNum );
    showSettings();
}

void KBackground::showSettings()
{ 
    colButton1->setColor( color1 );
    colButton2->setColor( color2 );
    ((QRadioButton *)ncGroup->find( OneColor ))->setChecked( ncMode == OneColor );
    ((QRadioButton *)ncGroup->find( TwoColor ))->setChecked( ncMode == TwoColor);

    slotColorMode( ncMode );

    ((QRadioButton *)stGroup->find( Gradient ))->setChecked( stMode==Gradient );
    ((QRadioButton *)stGroup->find( Pattern ))->setChecked( stMode==Pattern );

    wpCombo->setCurrentItem( 0 );
    for ( int i = 1; i < wpCombo->count(); i++ )
    {
	if ( wallpaper == wpCombo->text( i ) )
	{
	    wpCombo->setCurrentItem( i );
	    break;
	}
    }

    if ( wallpaper != NO_WALLPAPER && wpCombo->currentItem() == 0 )
    {
	wpCombo->insertItem( wallpaper );
	wpCombo->setCurrentItem( wpCombo->count()-1 );
    }

    ((QRadioButton *)wpGroup->find( Tiled ))->setChecked( wpMode == Tiled );
    ((QRadioButton *)wpGroup->find( Centred ))->setChecked( wpMode == Centred );
    ((QRadioButton *)wpGroup->find( Scaled ))->setChecked( wpMode == Scaled );

    setMonitor();
}

void KBackground::slotApply()
{
	writeSettings( deskNum );
	KApplication::getKApplication()->getConfig()->sync();
	apply( true );
}

void KBackground::apply( bool force )
{
	if ( !changed && !force )
		return;

	// tell background module to reload settings
	QString cmd = "kbgwm_reconfigure";
	KWM::sendKWMCommand( cmd );
}

void KBackground::retainResources() {
	// Retain server resources after the client exits.
	Display *dpy;
	dpy = x11Display();
	XKillClient(dpy, AllTemporary);
	XSetCloseDownMode(dpy, RetainTemporary);
}

void KBackground::setMonitor()
{
    float sx = (float)monitor->width() / 
	QApplication::desktop()->width();
    float sy = (float)monitor->height() / 
	QApplication::desktop()->height();
    QWMatrix matrix;
    matrix.scale( sx, sy );

    
    if ( wpPixmap.isNull() || wpMode == Centred || wallpaper == NO_WALLPAPER) {

	KPixmap preview;
	preview.resize( monitor->width()+1, monitor->height()+1 );
	
	if ( ncMode == OneColor ) {
		preview.fill( color1 );
	} else if ( ncMode == TwoColor && stMode == Gradient ) {
		
		if( orMode == Portrait ) 
			preview.gradientFill( color2, color1, true );
	    else
			preview.gradientFill( color2, color1, false );
			
	} else if ( ncMode == TwoColor && stMode == Pattern ) {
		preview.patternFill(color1, color2, pattern);
	}
	
	if ( wpMode == Centred && wallpaper != NO_WALLPAPER ) {
		QPixmap tmp = wpPixmap.xForm( matrix );

		bitBlt( &preview, (preview.width() - tmp.width())/2,
			(preview.height() - tmp.height())/2, &tmp,
			0, 0, tmp.width(), tmp.height() );
	}
	
	monitor->setBackgroundPixmap( preview );

    } else if ( !wpPixmap.isNull() ) 
	monitor->setBackgroundPixmap( wpPixmap.xForm( matrix ) );
    
}

// Attempts to load the specified wallpaper and creates a centred/scaled
// version if necessary.
// Note that centred pixmaps are placed on a full screen image of background
// color1, so if you want to save memory use a small tiled pixmap.
//
int KBackground::loadWallpaper( const char *name, bool useContext )
{
	static int context = 0;
	QString filename;
	int rv = false;
	KPixmap tmp;

	QApplication::setOverrideCursor( waitCursor );

	if ( useContext )
	{
	    if ( context )
		QColor::destroyAllocContext( context );
	    context = QColor::enterAllocContext();
	}

	if ( name[0] != '/' )
	{
	    filename = kapp->kde_wallpaperdir().copy();
	    filename += "/";
	    filename += name;
	}
	else
	    filename = name;
	
	if ( tmp.load( filename, 0, KPixmap::LowColor ) == true )
	{
		int w = QApplication::desktop()->width();
		int h = QApplication::desktop()->height();

		switch ( wpMode ) {

		case Centred:
		    wpPixmap = tmp;
		    break;
		    
		case Scaled: {
		    float sx = (float)w / tmp.width();
		    float sy = (float)h / tmp.height();
		    QWMatrix matrix;
		    matrix.scale( sx, sy );
		    wpPixmap = tmp.xForm( matrix );
		}
		break;
		
		default:
		    wpPixmap = tmp;
		}
		rv = true;
	}

	if ( useContext )
		QColor::leaveAllocContext();

	QApplication::restoreOverrideCursor();

	return rv;
}

void KBackground::slotSelectColor1( const QColor &col )
{
	color1 = col;

	if ( ncMode == Gradient || wpMode == Centred || wallpaper == NO_WALLPAPER )
	{
		// force the background to be made with different background
		if ( wpMode == Centred )
		    loadWallpaper( wallpaper );
		setMonitor();
	}

	changed = true;
}

void KBackground::slotSelectColor2( const QColor &col )
{
	color2 = col;

	if ( ncMode == Gradient || wpMode == Centred || wallpaper == NO_WALLPAPER )
	{
		setMonitor();
	}

	changed = true;
}

void KBackground::slotBrowse()
{
	QString path;

	static bool firsttime = true;

	if (firsttime) { // the file selector remembers the last path
	    path = kapp->kde_wallpaperdir().copy();

	    QDir dir( path );
	    if ( !dir.exists() )
		path = 0;
	    
	    firsttime = false;
	}

	QString filename = QFileDialog::getOpenFileName( path );
	slotWallpaper( filename );
	if ( !filename.isNull() && !strcmp( filename, wallpaper) )
	{
		wpCombo->insertItem( wallpaper );
		wpCombo->setCurrentItem( wpCombo->count() - 1 );
	}
}

void KBackground::slotWallpaper( const char *filename )
{
    if ( filename )
	{
	    if ( !strcmp( filename, NO_WALLPAPER ) )
		{
		    wpPixmap.resize(0, 0); // make NULL pixmap
		    wallpaper = filename;
		    setMonitor();
		}
	    else if ( loadWallpaper( filename ) == true )
		{
		    wallpaper = filename;
		    setMonitor();
		}
	    
	    changed = true;
	}
}

void KBackground::slotWallpaperMode( int m )
{
	wpMode = m;
	if ( loadWallpaper( wallpaper ) == true )
	{
		setMonitor();
		changed = true;
	}
}

void KBackground::slotColorMode( int m )
{
	ncMode = m;

	switch ( ncMode ) {

	case OneColor:
	    
	    rbPattern->setEnabled( False );
	    rbGradient->setEnabled( False );
	    colButton2->setEnabled( False );
	    changeButton->setEnabled( False );
	    break;
	
	default:
	    
	    rbPattern->setEnabled( True );
	    rbGradient->setEnabled( True );
	    colButton2->setEnabled( True );
	    changeButton->setEnabled( True );
	    break;
	}

	setMonitor();
	changed = true;
}

void KBackground::slotSetup2Color()
{
    KBPatternDlg dlg(color1, color2, pattern, &orMode, this);
    if (dlg.exec()) {
	setMonitor();
	changed = true;
    } 
}

void KBackground::slotStyleMode( int m )
{
    stMode = m;
    setMonitor();
    changed = true;
}

void KBackground::slotSwitchDesk( int num )
{
    setDesktop( num );
}

void KBackground::slotRenameDesk()
{
    KRenameDeskDlg dlg( KWM::getDesktopName( deskNum+1 ), this );

    dlg.setCaption( i18n( "Desktop names" ) );

    if ( dlg.exec() == QDialog::Accepted )
    {
	KWM::setDesktopName( deskNum+1, dlg.title() );
	getDeskNameList();
    }
}

void KBackground::slotHelp()
{
    kapp->invokeHTMLHelp( "kdisplay/kdisplay-3.html", "" );
}

void KBackground::loadSettings()
{
    readSettings(deskNum);
    showSettings();
}

void KBackground::applySettings()
{
    writeSettings(deskNum);
    apply(true);
}


KBPatternDlg::KBPatternDlg( QColor col1, QColor col2, uint *p, int *mode,
			    QWidget *parent, char *name)
    : QDialog( parent, name, true)
{
    int i;
    pattern = p;
	orMode = mode;
    color1 = col1; 
    color2 = col2;
	
	setCaption( i18n( "Two color background setup" ) );

    KConfig *config = kapp->getConfig();
    QString group = config->group();
    config->setGroup( "Defined Pattern" );
    int count = config->readNumEntry( "Count" );

    if (!count)
	count = savePatterns();

    QString name;
    for (i = 0; i < count; i++) {
	PatternEntry *entry = new PatternEntry();
	name.sprintf("Name%d",i);
	entry->name = config->readEntry( name );
	if (entry->name.isNull()) {
	    delete entry;
	    continue;
	}
	QStrList strl;
	name.sprintf("Pattern%d",i);
	config->readListEntry(name, strl);
	uint size = strl.count();
	if (size > 8) size = 8;
	for (uint j = 0; j < 8; j++)
	    entry->pattern[j] = (j < size) ? 
		QString(strl.at(j)).toUInt() : 255;
	list.append(entry);
    }

    QVBoxLayout *toplevelHL = new QVBoxLayout(this, 10);
    QGridLayout *grid = new QGridLayout( 4, 4, 5);
    
    toplevelHL->addLayout( grid );
	
	grid->setRowStretch(0,0);
	grid->setRowStretch(1,10);
	grid->setRowStretch(2,10);
	grid->setRowStretch(3,0);
	
	grid->setColStretch(0,0);
	grid->setColStretch(1,10);
	grid->setColStretch(2,10);
	grid->setColStretch(3,0);
    
	QLabel *label = new QLabel( this );
	label->setText( i18n( "Pattern name" ) );
	label->setMinimumSize( label->sizeHint() );
	
	grid->addWidget( label, 1, 1 );
	
	
	label = new QLabel( this );
	label->setText( i18n( "Preview" ) );
	label->setMinimumSize( label->sizeHint() );
	
	grid->addWidget( label, 1, 2 );
	
    listBox = new QListBox( this );
    connect(listBox, SIGNAL(highlighted(const char*)), 
	    SLOT(selected(const char*)));
    
	grid->addWidget( listBox, 2, 1);
    
    preview = new QLabel( this );
    preview->adjustSize();
    preview->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	preview->setLineWidth( 1 );
	preview->setMargin( 0 );
    preview->setFixedSize( 120, 120 );
    
	grid->addWidget( preview, 2, 2 );
    
	listBox->setMaximumHeight( preview->height() );
	
	orientCB = new QCheckBox( i18n("&Blend colors from right to left"), this );
	orientCB->setFixedHeight( orientCB->sizeHint().height() );
	if( *orMode == KBackground::Landscape ) {
		orientCB->setChecked( True );
	} else {
		orientCB->setChecked( False );
	}
	
	toplevelHL->addWidget( orientCB );
   
   	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( this );
	tmpQFrame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	tmpQFrame->setMinimumHeight( tmpQFrame->sizeHint().height() );
	
	toplevelHL->addWidget( tmpQFrame );
   
	KButtonBox *bbox = new KButtonBox( this );
	bbox->addStretch( 10 );
	
	QPushButton* okPB;
	okPB = bbox->addButton( i18n("OK") );
	connect( okPB, SIGNAL(clicked()), SLOT( accept()) );
	
	QPushButton* cancelPB;
	cancelPB = bbox->addButton( i18n("Cancel") );
	connect( cancelPB, SIGNAL(clicked()), SLOT( reject()) );
	
	bbox->layout();
	toplevelHL->addWidget( bbox );

    current = new PatternEntry( i18n("Current"), p);
    bool predefined = false;
    for ( PatternEntry *item=list.first(); item != 0; item=list.next() )
	if (*current == item->pattern ) {
	    delete current;
	    current = item;
	    predefined = true;
	    break;
	}
    
    if (!predefined) {
	listBox->insertItem( current->name );
	listBox->setCurrentItem(0);
    }

    for ( PatternEntry *item=list.first(); item != 0; item=list.next() )
	listBox->insertItem(item->name);

    // we do this here to let all items fit in
    listBox->adjustSize();
    listBox->setMinimumSize(listBox->size());

    toplevelHL->activate();
    resize(10, 10);

    selected( current->name );
}

int KBPatternDlg::savePatterns() {
    KConfig *config = kapp->getConfig();

    config->writeEntry("Name0", i18n("Filled"), true, false, true);
    config->writeEntry("Pattern0", "255,255,255,255,255,255,255,255,");
    
    config->writeEntry("Name1", i18n("Fish net"), true, false, true);
    config->writeEntry("Pattern1", "135,206,236,120,062,103,115,225,");

    config->writeEntry("Name2", i18n("Triangles"), true, false, true);
    config->writeEntry("Pattern2", "120,248,249,251,255,8,24,56,");

    config->writeEntry("Name3", i18n("Flowers"), true, false, true);
    config->writeEntry("Pattern3", "225,115,39,2,64,228,206,135,");

    config->writeEntry("Name4", i18n("Rattan"), true, false, true);
    config->writeEntry("Pattern4", "7,139,221,184,112,232,221,142,");

    config->writeEntry("Name5", i18n("Cobbled Pavement"), true, false, true);
    config->writeEntry("Pattern5", "81,178,160,247,178,81,178,");

    config->writeEntry("Count", 6);

    config->sync();
    return 6;
}

void KBPatternDlg::selected( const char *selected)
{
    for ( PatternEntry *item=list.first(); item != 0; item=list.next() )
	if (*item == selected) {
	    KPixmap tmp;
	    tmp.resize(preview->width() + 2, preview->height() + 2);
	    tmp.patternFill( color1, color2, item->pattern);
	    preview->setPixmap(tmp);
	    current = item;
	    break;
	}
}

void KBPatternDlg::done( int r ) 
{
    hide();
	
	int orient;
	
	if( orientCB->isChecked() ) {
		orient = KBackground::Landscape;
	} else {
		orient = KBackground::Portrait;
	}
	    
    if ( r == Rejected || ( *current == pattern && *orMode == orient ) ) {
		setResult(Rejected);
		return;
    }
	
	*orMode = orient;
    
	for (uint i = 0; i < 8; i++)
		pattern[i] = current->pattern[i];

    setResult(Accepted);
    
}
