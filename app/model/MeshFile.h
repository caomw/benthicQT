/* Copyright 2010 NVIDIA Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Developed by Mustard Seed Software, LLC 
 * http://mseedsoft.com
 */

#ifndef __MESHFILE_H
#define __MESHFILE_H


#include <QtCore>
#include <QObject>
#include <osg/Vec3>

#include <osg/Vec2>
#include <osg/ref_ptr>
using osg::ref_ptr;
#include "BQTDefine.h"
#include "MathUtils.h"

namespace ews {
    namespace app {
        namespace model {
            using osg::Vec2;

                        
            
            /**
             * Contains the business logic for barrier objects to be drawn on the screen, as well as
             * a reference to the SlitPotential necessary for physics calculations.
             */
            class MeshFile :  public QObject  {
                Q_OBJECT
                Q_PROPERTY(QStringList filenames READ getFileNames WRITE setFileNames)
                
            public:

                 void loadMesh();
                /**
                 * Default constructor.
                 * @param parent BarrierSet this Barrier belongs to.
                 */
                explicit MeshFile();
                
                virtual ~MeshFile();

                


                /**
                 * Gets enabled state of barrier. 
                 * @return true if active, false if inactive and invisible.
                 */
                bool isEnabled() const {
                    return _enabled;
                }
                

                void setFileNames(QStringList files) {
                    filenames = files;
                    emit dataChanged();
                }

                QStringList getFileNames(void) {
                    return filenames ;

                }
                

                void updatePos(osg::Vec3 v) {
                    emit posChanged(v);
                }


            signals:
                void dataChanged();
                //void potentialChanged();
                void loadNewMeshes(MeshFile *);

                /**
                 * General signal for case when number of items in the
                 * set has changed. Parameter is the new number of items.
                 */
                void posChanged(osg::Vec3);

            private slots:
              //  void generatePotential();
                
            private:
                Q_DISABLE_COPY(MeshFile)

                bool _enabled;
                QStringList filenames;
            };
        }
    }
}


#endif // __BARRIER_H

