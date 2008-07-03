#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include <Scene/TransformationNode.h>
#include <Resources/ResourceManager.h>

#include <Renderers/OpenGL/TextureLoader.h>
#include <Resources/ITextureResource.h>
#include <Geometry/FaceSet.h>
#include <Scene/GeometryNode.h>
#include <Math/Vector.h>
#include <string.h>
#include <Resources/IMovieResource.h>

using namespace std;
using namespace OpenEngine::Geometry;

using OpenEngine::Resources::ResourceManager;
using OpenEngine::Resources::ITextureResourcePtr;
using OpenEngine::Geometry::FaceSet;
using OpenEngine::Math::Vector;
using OpenEngine::Renderers::OpenGL::TextureLoader;
using OpenEngine::Scene::GeometryNode;
using OpenEngine::Scene::TransformationNode;

class Billboard {
public:
    static TransformationNode* Create(string textureFile, int textureHosisontalSize, int textureVerticalSize, float scale) {
        FaceSet* faces = new FaceSet();

        float horisontalhalfsize = textureHosisontalSize * 0.5;
        Vector<3,float>* lowerleft = new Vector<3,float>(horisontalhalfsize,-textureVerticalSize*0.5,0);
        Vector<3,float>* lowerright = new Vector<3,float>(-horisontalhalfsize,-textureVerticalSize*0.5,0);
        Vector<3,float>* upperleft = new Vector<3,float>(horisontalhalfsize,textureVerticalSize*0.5,0);
        Vector<3,float>* upperright = new Vector<3,float>(-horisontalhalfsize,textureVerticalSize*0.5,0);
        
        FacePtr leftside = FacePtr(new Face(*lowerleft,*lowerright,*upperleft));
        leftside->texc[1] = Vector<2,float>(1,0);
        leftside->texc[0] = Vector<2,float>(0,0);
        leftside->texc[2] = Vector<2,float>(0,1);
        leftside->norm[0] = leftside->norm[1] = leftside->norm[2] = Vector<3,float>(0,0,-1);
        leftside->CalcHardNorm();
        leftside->Scale(scale);
        faces->Add(leftside);
        
        FacePtr rightside = FacePtr(new Face(*lowerright,*upperright,*upperleft));
        rightside->texc[2] = Vector<2,float>(0,1);
        rightside->texc[1] = Vector<2,float>(1,1);
        rightside->texc[0] = Vector<2,float>(1,0);
        rightside->norm[0] = rightside->norm[1] = rightside->norm[2] = Vector<3,float>(0,0,-1);
        rightside->CalcHardNorm();
        rightside->Scale(scale);
        faces->Add(rightside);
        
        MaterialPtr m = MaterialPtr(new Material());
	m->texr = ResourceManager<ITextureResource>::Create(textureFile);
	leftside->mat = rightside->mat = m;
        
        GeometryNode* node = new GeometryNode();
        node->SetFaceSet(faces);
        TransformationNode* tnode = new TransformationNode();
        tnode->AddNode(node);

        TextureLoader texLoad;
        node->Accept(texLoad);

        return tnode;
    }

    static TransformationNode* CreateMovieBillboard(IMovieResourcePtr movie, float scale) {
        unsigned int textureHosisontalSize = movie->GetMovieWidth();
        unsigned int textureVerticalSize = movie->GetMovieHeight();
        float fullxtexcoord = (float)movie->GetMovieWidth()/(float)movie->GetWidth();
        float fullytexcoord = (float)movie->GetMovieHeight()/(float)movie->GetHeight();

        FaceSet* faces = new FaceSet();

        float horisontalhalfsize = textureHosisontalSize * 0.5;
        Vector<3,float>* lowerleft = new Vector<3,float>(horisontalhalfsize,0,0);
        Vector<3,float>* lowerright = new Vector<3,float>(-horisontalhalfsize,0,0);
        Vector<3,float>* upperleft = new Vector<3,float>(horisontalhalfsize,textureVerticalSize,0);
        Vector<3,float>* upperright = new Vector<3,float>(-horisontalhalfsize,textureVerticalSize,0);

        FacePtr leftside = FacePtr(new Face(*lowerleft,*lowerright,*upperleft));

        /*
          leftside->texc[1] = Vector<2,float>(1,0);
          leftside->texc[0] = Vector<2,float>(0,0);
          leftside->texc[2] = Vector<2,float>(0,1);
        */
        leftside->texc[1] = Vector<2,float>(0,fullytexcoord);
        leftside->texc[0] = Vector<2,float>(fullxtexcoord,fullytexcoord);
        leftside->texc[2] = Vector<2,float>(fullxtexcoord,0);
        leftside->norm[0] = leftside->norm[1] = leftside->norm[2] = Vector<3,float>(0,0,1);
        leftside->CalcHardNorm();
        leftside->Scale(scale);
        faces->Add(leftside);

        FacePtr rightside = FacePtr(new Face(*lowerright,*upperright,*upperleft));
        /*
          rightside->texc[2] = Vector<2,float>(0,1);
          rightside->texc[1] = Vector<2,float>(1,1);
          rightside->texc[0] = Vector<2,float>(1,0);
        */
        rightside->texc[2] = Vector<2,float>(fullxtexcoord,0);
        rightside->texc[1] = Vector<2,float>(0,0);
        rightside->texc[0] = Vector<2,float>(0,fullytexcoord);
        rightside->norm[0] = rightside->norm[1] = rightside->norm[2] = Vector<3,float>(0,0,1);
        rightside->CalcHardNorm();
        rightside->Scale(scale);
        faces->Add(rightside);

        MaterialPtr m = leftside->mat = rightside->mat = MaterialPtr(new Material());
	m->texr = movie;

        GeometryNode* node = new GeometryNode();
        node->SetFaceSet(faces);
        TransformationNode* tnode = new TransformationNode();
        tnode->AddNode(node);
        return tnode;
    }

};

#endif
