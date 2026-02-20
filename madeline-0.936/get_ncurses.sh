#!/bin/sh
#
#
# *********************************************************************************
#  _______________________________________________________________________________ 
# |_______________________________________________________________________________|
#   __    __       _       ______     _______   _          _   __    _   _______   
#  |  \  /  |     / \     |  ___  \  |  _____| | |        | | |  \  | | |  _____|  
#  |   \/   |    / ^ \    | |   \  \ | |___    | |        | | |   \ | | | |___     
#  | |\  /| |   / /_\ \   | |    | | |  ___|   | |        | | | |\ \| | |  ___|    
#  | | \/ | |  /  ___  \  | |___/  / | |_____  | |______  | | | | \   | | |_____   
#  |_|    |_| /__/   \__\ |_______/  |_______| |________| |_| |_|  \__| |_______|  
#  _______________________________________________________________________________ 
# |_______________________________________________________________________________|
#                                                                                  
# "get_ncurses.sh" is part of the Madeline software distribution.                 
# Written by Edward H. Trager.                                                     
#                                                                                  
# COPYRIGHT (C) 2001 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                    
# Ann Arbor, Michigan, USA.  ALL RIGHTS RESERVED.                                  
#                                                                                  
# This program is free software; you can redistribute it and/or                    
# modify it under the terms of the GNU General Public                              
# License as published by the Free Software Foundation; either                     
# version 2 of the License, or (at your option) any later version.                 
#                                                                                  
# This program is distributed in the hope that it will be useful,                  
# but WITHOUT ANY WARRANTY; without even the implied warranty of                   
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
# Library General Public License for more details.                                 
#                                                                                  
# You should have received a copy of the GNU Library General Public                
# License along with this software source code distribution; if not,               
# write to the Free Software Foundation, Inc., 59 Temple Place -                   
# Suite 330, Boston, MA 02111-1307, USA.                                           
#                                                                                  
# Last modified by Ed Trager, December 2001.                                       
#                                                                                  
# *********************************************************************************
#
# get_ncurses.sh: script to retrieve and install the GNU ncurses 
# library from the GNU ftp server.
#
# -- 2001.12.21 -- Edward Trager
#
PWD=`pwd`
if test "X$PREFIX" = "X"
then echo "********************************************************************"
     echo "**** Script needs to be called with \$PREFIX set to specify the  ****"
     echo "**** installation prefix for the GNU ncurses library.           ****"
     echo "****                                                            ****"
     echo "**** i.e.: \"PREFIX=/home/cat ./get_ncurses.sh\"                  ****" 
     echo "********************************************************************" 
     exit 1
else
     if test $PREFIX = "NONE"
     then PREFIX="/usr/local"
     fi
fi
if test -x `which wget`
then ANSWER=z
     while test $ANSWER != y -a $ANSWER != n
     do echo "********************************************************************"
        echo "****                 ** get_ncurses.sh **                       ****"
        echo "****                 ---------------------                      ****"
        echo "****                                                            ****"
        echo "**** Assuming that you have the necessary permissions,          ****"
        echo "**** this script can download and install the GNU ncurses       ****"
        echo "**** library in \"$PREFIX\""
        echo "**** for you now.  Would you like to do this now?(y/n)          ****"
        echo "********************************************************************"
        if test $ANSWER != z
        then echo "Hmmm ... I like \"$ANSWER\" too!"
             echo "However, you need to answer either \"y\" (yes) or \"n\" (no):" 
        else echo "(y/n):"
        fi
        read ANSWER
     done
     
     if test -z $ANSWER
     then echo "********************************************"
          echo "**** So, you don't feel like answering! ****"
          echo "**** No hard feelings! Goodbye!         ****"
          echo "********************************************" 
          exit 1
     fi

     if test $ANSWER = n
     then echo "************************************************"
          echo "**** You have chosen to not run the script. ****"
          echo "**** OK ... Goodbye!                        ****"
          echo "************************************************" 
          exit 1
     fi
     
     echo "********************************************************************"
     echo "**** Starting attempt to download ncurses library ...           ****"
     echo "********************************************************************"
     cd .. &&  wget ftp://ftp.gnu.org/gnu/ncurses/ncurses-5.2.tar.gz
     if test $? -eq 0
     then echo "***************************************************************"
          echo "**** Download successful ...                               ****"
          echo "***************************************************************"
     else
          echo "***************************************************************"
          echo "**** Sorry, couldn't download file.  You will have to do   ****"
          echo "**** it the old-fashioned manual way ...                   ****"
          echo "***************************************************************"
          cd $PWD
          exit 1
     fi
     echo "********************************************************************"
     echo "**** Expanding file archive ...                                 ****"
     echo "********************************************************************" 
     gzip -cd ncurses-5.2.tar.gz | tar xvf -
     if test $? -eq 0
     then echo "***************************************************************"
          echo "**** Archive looks OK ...                                  ****"
          echo "**** Entering directory to run configure and make ...      ****"
          echo "***************************************************************"
     else
          echo "***************************************************************"
          echo "**** Sorry, couldn't expand archive!  You will have to do  ****"
          echo "**** it the old-fashioned manual way ...                   ****"
          echo "***************************************************************"
          cd $PWD
          exit 1
     fi
     cd ncurses-5.2 && ./configure --prefix=$PREFIX
     if test $? -eq 0
     then echo "***************************************************************"
          echo "**** Configuration of GNU ncurses looks OK ...             ****"
          echo "**** Running make to compile the library next ...          ****"
          echo "***************************************************************"
     else
          echo "***************************************************************"
          echo "**** Sorry, There's a configuration issue!  You will have  ****"
          echo "**** to do it the old-fashioned manual way ...             ****"
          echo "***************************************************************"
          cd $PWD
          exit 1
     fi
     make
     if test $? -eq 0
     then echo "***************************************************************"
          echo "**** Make successfully completed compiling GNU ncurses.    ****"
          echo "**** Running make install to install the library next ...  ****"
          echo "***************************************************************"
     else
          echo "***************************************************************"
          echo "**** Sorry, There's a compilation issue!  You will have    ****"
          echo "**** to do it the old-fashioned manual way ...             ****"
          echo "***************************************************************"
          cd $PWD
          exit 1
     fi
     make install
     if test $? -eq 0
     then echo "**********************************************************************" 
          echo "**** GNU ncurses v. 5.2 downloaded and installed successfully !!  ****"
          echo "****                                                              ****"
          echo "**** Now try executing Madeline's \"./configure\" one more time:  ****" 
          echo "****                                                              ****"
          echo "**** ./configure --prefix=$PREFIX"
          echo "****                                                              ****"
          echo "**** You should not encounter any problems.  However, if you do   ****"
          echo "**** and you are using a non-default prefix, then be sure that    ****"
          echo "**** \$LD_LIBRARY_PATH is properly set by doing something like the ****"
          echo "**** following (The exact command depends on what shell you use.  ****"
          echo "**** This works for Bourne and Bourne Again shells):              ****"
          echo "****                                                              ****"
          echo "**** LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$PREFIX/lib"
          echo "**** export LD_LIBRARY_PATH                                       ****"
          echo "****                                                              ****"          
          echo "**********************************************************************"
          cd $PWD
          exit 0
     else
          echo "******************************************************************************"
          echo "**** Sorry, script did not complete installation of GNU ncurses !         ****"
          echo "**** I guess you will just have to do it the old-fashioned manual way ... ****"
          echo "******************************************************************************"
          cd $PWD
          exit 1
     fi
else
     echo "******************************************************************************"
     echo "**** Sorry, this script could not find the 'wget' command for downloading ****"
     echo "**** the required file. 'wget' is a GNU utility.  Ask your site admini-   ****"
     echo "**** strator (Is that you?) to provide more of the GNU utilities in the   ****"
     echo "**** future.  For now, I guess you will just have to do it the old-       ****"
     echo "**** fashioned manual way ...                                             ****"
     echo "******************************************************************************"
     cd $PWD
     exit 1
fi

