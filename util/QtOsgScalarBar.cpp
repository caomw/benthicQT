#include "QtOsgScalarBar.h"
#include <osgText/Text>
#include <osg/Geometry>
#include <osg/Version>
#include <sstream>
#include <stdio.h>
using namespace myOSG;
using namespace osgSim;

QtOsgScalarBar::~QtOsgScalarBar()
{
}

std::string QtOsgScalarBar::ScalarPrinter::printScalar(float scalar)
{
    std::stringstream ostr;
    ostr<<scalar;
    return ostr.str();
}

void QtOsgScalarBar::setNumColors(int numColors)
{
    _numColors = numColors;
    createDrawables();
}

int QtOsgScalarBar::getNumColors() const
{
    return _numColors;
}

void QtOsgScalarBar::setNumLabels(int numLabels)
{
    _numLabels = numLabels;
    createDrawables();
}

int QtOsgScalarBar::getNumLabels() const
{
    return _numLabels;
}

void QtOsgScalarBar::setScalarsToColors(ScalarsToColors* stc)
{
    _stc = stc;
    createDrawables();
}

const ScalarsToColors* QtOsgScalarBar::getScalarsToColors() const
{
    return _stc.get();
}

void QtOsgScalarBar::setTitle(const std::string& title)
{
    _title = title;
    createDrawables();
}

const std::string& QtOsgScalarBar::getTitle() const
{
    return _title;
}

void QtOsgScalarBar::setPosition(const osg::Vec3& pos)
{
    _position = pos;
    createDrawables();
}

void QtOsgScalarBar::setWidth(float width)
{
    _width = width;
    createDrawables();
}

void QtOsgScalarBar::setOrientation(QtOsgScalarBar::Orientation orientation)
{
    _orientation = orientation;
    createDrawables();
}

QtOsgScalarBar::Orientation QtOsgScalarBar::getOrientation() const
{
    return _orientation;
}

void QtOsgScalarBar::setAspectRatio(float aspectRatio)
{
    _aspectRatio = aspectRatio;
    createDrawables();
}

float QtOsgScalarBar::getAspectRatio() const
{
    return _aspectRatio;
}

void QtOsgScalarBar::setScalarPrinter(ScalarPrinter* sp)
{
    _sp = sp;
    createDrawables();
}

const QtOsgScalarBar::ScalarPrinter* QtOsgScalarBar::getScalarPrinter() const
{
    return _sp.get();
}


void QtOsgScalarBar::setTextProperties(const TextProperties& tp)
{
    _textProperties = tp;
    createDrawables();
}

const QtOsgScalarBar::TextProperties& QtOsgScalarBar::getTextProperties() const
{
    return _textProperties;
}


void QtOsgScalarBar::createDrawables()
{
    if(_font==NULL){
        fprintf(stderr,"Font null\n");
        return;
    }
    // Remove any existing Drawables
    _drawables.erase(_drawables.begin(), _drawables.end());

    if (_numColors==0) return;

    osg::Matrix matrix;
    if(_orientation==HORIZONTAL)
    {
        matrix = osg::Matrix::translate(_position);
    }
    else
    {
        matrix = osg::Matrix::rotate(osg::DegreesToRadians(90.0f),1.0f,0.0f,0.0f) * osg::Matrix::translate(_position);
    }

    // 1. First the bar
    // =================
    osg::ref_ptr<osg::Geometry> bar = new osg::Geometry();

    // Create the bar - created in 'real' coordinate space the moment,
    // with xyz values reflecting those of the actual scalar values in play.
    // FIXME: Consider positioning at origin! Should be easy enough to do.

    // Vertices
    osg::ref_ptr<osg::Vec3Array> vs(new osg::Vec3Array);
    vs->reserve(2*(_numColors+1));

    float incr = (_stc->getMax() - _stc->getMin()) / _numColors;
    float xincr = (_width) / _numColors;
    float arOffset = _width * _aspectRatio;

    int i;
    for(i=1; i<=_numColors; ++i)
    {
        vs->push_back(osg::Vec3((i-1) * xincr, 0.0f,     0.0f)*matrix);
        vs->push_back(osg::Vec3((i-1) * xincr, arOffset, 0.0f)*matrix);
        vs->push_back(osg::Vec3(i     * xincr, arOffset, 0.0f)*matrix);
        vs->push_back(osg::Vec3(i     * xincr, 0.0f,     0.0f)*matrix);
    }
    bar->setVertexArray(vs.get());

    // Colours
    osg::ref_ptr<osg::Vec4Array> cs(new osg::Vec4Array);
    cs->reserve(_numColors);
    const float halfIncr = incr*0.5;
    for(i=0; i<_numColors; ++i)
    {
        // We add half an increment to the color look-up to get the color
        // square in the middle of the 'block'.
#if OPENSCENEGRAPH_MAJOR_VERSION < 3 || (OPENSCENEGRAPH_MAJOR_VERSION == 3 && OPENSCENEGRAPH_MINOR_VERSION <= 1)

        cs->push_back(_stc->getColor(_stc->getMin() + (i*incr)  + halfIncr));
      #else
        for(int j=0; j< 4; j++)
            cs->push_back(_stc->getColor(_stc->getMin() + (i*incr)  + halfIncr));
#endif
    }
    bar->setColorArray(cs.get());

#if OPENSCENEGRAPH_MAJOR_VERSION < 3 || (OPENSCENEGRAPH_MAJOR_VERSION == 3 && OPENSCENEGRAPH_MINOR_VERSION <= 1)
    bar->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
 #else
    bar->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

 #endif


    // Normal
    osg::ref_ptr<osg::Vec3Array> ns(new osg::Vec3Array);
    ns->push_back(osg::Matrix::transform3x3(osg::Vec3(0.0f,0.0f,1.0f),matrix));
    bar->setNormalArray(ns.get());
    bar->setNormalBinding(osg::Geometry::BIND_OVERALL);

    // The Quad strip that represents the bar
    bar->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0,vs->size()));

    addDrawable(bar.get());

    // 2. Then the text labels
    // =======================

    // Check the character size, if it's 0, estimate a good character size
    float characterSize = _textProperties._characterSize;
    if(characterSize == 0) characterSize = _width * 0.075f;

    //osgText::Font* font = osgText::readFontFile(_textProperties._fontFile.c_str());

    std::vector<osgText::Text*> texts(_numLabels);      // We'll need to collect pointers to these for later
    float labelIncr = (_numLabels>0) ? (_stc->getMax()-_stc->getMin())/(_numLabels-1) : 0.0f;
    float labelxIncr = (_numLabels>0) ? (_width)/(_numLabels-1) : 0.0f;
    float labely = arOffset + characterSize*0.3f;
    for(i=0; i<_numLabels; ++i)
    {
        osgText::Text* text = new osgText::Text;
        text->setFont(_font);
        text->setColor(_textProperties._color);
       // text->setFontResolution(_textProperties._fontResolution.first,_textProperties._fontResolution.second);
        text->setCharacterSize(characterSize);
        text->setText(_sp->printScalar(_stc->getMin()+(i*labelIncr)));

        text->setPosition(osg::Vec3((i*labelxIncr), labely, 0.0f)*matrix);
        text->setAlignment(osgText::Text::CENTER_BASE_LINE);
        text->setAxisAlignment( (_orientation==HORIZONTAL) ? osgText::Text::XY_PLANE :  osgText::Text::XZ_PLANE );

        addDrawable(text);

        texts[i] = text;
    }


    // 3. And finally the title
    // ========================

    if(_title != "")
    {
        osgText::Text* text = new osgText::Text;
        text->setFont(_font);
        text->setColor(_textProperties._color);
        text->setFontResolution(_textProperties._fontResolution.first,_textProperties._fontResolution.second);
        text->setCharacterSize(characterSize);
        text->setText(_title);

        float titleY = (_numLabels>0) ? labely + characterSize : labely;

        // Position the title at the middle of the bar above any labels.
        text->setPosition(osg::Vec3((_width/2.0f), titleY, 0.0f)*matrix);
        text->setAlignment(osgText::Text::CENTER_BASE_LINE);
        text->setAxisAlignment( (_orientation==HORIZONTAL) ? osgText::Text::XY_PLANE :  osgText::Text::XZ_PLANE );

        addDrawable(text);
    }
}

