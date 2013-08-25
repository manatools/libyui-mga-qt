##### PROJECTINFO for CMake

SET( BASELIB		"yui" )		# don't change this

##### MAKE ALL NEEDED CHANGES HERE #####

SET( SUBDIRS		src )	        # set the subdirs where your code is located.
SET( PLUGINNAME		"mga-qt" )		# define the your plugin's name, e.g. {gtk, ncurses-pkg, qt-graph}
SET( LIB_DEPS		Qt )		# define the plugin's dependecies CMake should look for separated with spaces; libyui-internal deps go in the next line
SET( INTERNAL_DEPS	Libyui )	# include libyui or it's plugins (if needed), too!
#SET( INTERNAL_DEPS	Libyui libyui-mga-qt )	# include libyui or it's plugins (if needed), too!
SET( LIB_LINKER		yui-mga yui-qt QtCore QtGui X11 fontconfig dl pthread )	# define the libs to link against with their -l name for separated with spaces, e.g. pthread dl; include libyui or it's plugins (if needed), too!
#SET( EXTRA_INCLUDES	)		# set include-dir which are not picked by CMake automagically here.
SET( PROGSUBDIR		"" )		# define your programm's name if the plugin is made for it, only. e.g. "YaST2" for {gtk,ncurses,qt}-pkg.
SET( URL                "http://TODO-mga" )
SET( SUMMARY            "Libyui - Mageia extension Qt User Interface" )
SET( DESCRIPTION        "This package contains the Qt user interface\ncomponent for libYUI Mageia extension.\n" )


## Things to cleanup; probably too yast2 related

SET( YUIQT_THEMEDIR		"${CMAKE_INSTALL_PREFIX}/share/YaST2/theme" )
SET( YUIQT_ICONDIR		"${CMAKE_INSTALL_PREFIX}/share/YaST2/theme/current" )
SET( YUIQT_LOCALEDIR		"${CMAKE_INSTALL_PREFIX}/share/YaST2/locale" )
SET( YUIQT_LANG_FONTS_FILE	"${CMAKE_INSTALL_PREFIX}/share/YaST2/data/lang_fonts" )
