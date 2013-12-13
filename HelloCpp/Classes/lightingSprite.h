

#ifndef __HelloCpp__lightingSprite__
#define __HelloCpp__lightingSprite__

#include <iostream>
#include "cocos2d.h"
#include "GLProgramWithMyUnifos.h"
#include "myFunc.h"
#include "simpleClasses.h"
#include "myDef.h"
using namespace cocos2d;
class ClineSeg{
public:
    CCPoint startPoint;
    CCPoint endPoint;
    CCPoint branchEndPoint;//only seed seg store this
    int branchID;//which branch the seg belongs to. branchID==0 means belongs to mainBranch
    int branchStartGenerationID;//in which generation the branch this seg belongs to start generated.
    int generationID;//this seg created in which generation, the original seg's generationID is 0
    ClineSeg*father;
    vector<ClineSeg*> childList;
    bool needReGenerate;//whether the branch use this seg as root seg need to regenerate
public:
    ClineSeg(){
        initParameter();
    }
    ClineSeg(CCPoint start,CCPoint end){
        initParameter();//do not forget this
        init(start, end);
    }
    virtual~ClineSeg(){}
    void init(CCPoint start,CCPoint end){
        startPoint=start;
        endPoint=end;
    }
    bool addChildRef(ClineSeg*child);
    bool removeChildRef(ClineSeg*child);
    float getLen()const {
        return ccpDistance(startPoint, endPoint);
    }
    CCPoint getDir()const {
        return ccpNormalize(endPoint-startPoint);
    }
    CCPoint getVec()const {
        return endPoint-startPoint;
    }
    CCPoint getRightNormal()const {//right side normal or outer normal
        CCPoint dir=getDir();
        CCPoint normal(dir.y,-dir.x);
        return normal;
    }
    CCPoint getMidPoint()const {
        return ccpMidpoint(startPoint, endPoint);
    }
private:
    void initParameter(){
        branchID=-1;
        branchStartGenerationID=-1;
        generationID=-1;
        father=NULL;
        branchEndPoint=CCPoint(0,0);
        needReGenerate=false;
    }
};

class Cmesh{
public:
    vector<Cv2> vlist;
    vector<Cv2> texCoordList;
    vector<CIDTriangle> IDtriList;
};
class CmeshWire{
public:
    vector<Cv2> vlist;
    vector<Cv2> texCoordList;
    vector<CIDLine> IDlineList;
};
enum eJitterMode{
    eJitterMode_noJitter=0,
    eJitterMode_reGenBranches,
    eJitterMode_adjuVert,
};
class ClightingSourceSprite:public CCSprite
{
public:
    bool init(string texFileName);
    
};
class ClightingSprite:public CCSprite
{
public:
    friend class Caction_lightingSprite_blinkSomeTime;//now Caction_lightingSprite_blinkSomeTime can access ClightingSprite's private member
public:
    ClightingSprite(){
        startPoint=CCPoint(0,0);
        endPoint=CCPoint(1000,0);
        mainSegLen=14;
        k_offset=0.13;//0.15;
        k_offsetGrow=1.2;//1.3
        updateNGeneration();
        dOpacity=10;
        isDebugDraw=false;
        jitterMode=eJitterMode_reGenBranches;
        headSeg=NULL;
        tailSeg=NULL;
        isDoJitter=true;
        howManyJitterInOneFade=3;//4
        percentageOfJitterBranch=0.5;//0.13;//0.2//0.13
        isDrawBrightLine=true;
        genBranchProbabilityFade=0.55;//0.6
    }
    virtual ~ClightingSprite(){
        clearSeges();
    }
public:
    void updateNGeneration();
    void draw();
    bool init();
    void update(float t);
    void setEndPoint(CCPoint endPointWorld);
    void setStartPoint(CCPoint startPointWorld);
    void genLighting();
    static void initProgram();
public://set/get
    void setDOpacity(float _dOpacity){dOpacity=_dOpacity;};
    float getDOpacity(){return dOpacity;};
    void setIsDebugDraw(bool value){isDebugDraw=value;};
    void setIsDoJitter(bool value){isDoJitter=value;};
    void setHowManyJitterInOneFade(int _howManyJitterInOneFade){howManyJitterInOneFade=_howManyJitterInOneFade;};
    void setPercentageOfJitterBranch(float percentage){percentageOfJitterBranch=percentage;};
    void setIsDrawBrightLine(bool value){isDrawBrightLine=value;};
    bool getIsDrawBrightLine(){return isDrawBrightLine;};
    void setK_offset(float _k_offset){k_offset=_k_offset;};
    void setGenBranchProbabilityFade(float k){genBranchProbabilityFade=k;}
private://generate lighting
    void genSeges();
    void genMesh();
    void clearSeges();
    vector<ClineSeg*> genSegesFromBranchSeg(ClineSeg*branchSeg,int nGeneration);
    vector<ClineSeg*> splitSeg(ClineSeg*seg,int generationID);
private://animate
    bool isFadeOut();
private://jitter
    void doJitter();
    void doJitter_reGenBranches();
    void releaseNeedReGenBranch(vector<ClineSeg*>&branchList,vector<ClineSeg*>&segList);
    void releaseNeedReGenBranch_inn(ClineSeg*seg,vector<ClineSeg*>&segList);
private://debug draw
    void debugDraw_segWireFrame(float lineWidth);
    void debugDraw_segLineAndDot(float lineWidth,float pointSize);
private:
    //shader program
    static CGLProgramWithMyUnifos program;
    //source and dest sprite
    ClightingSourceSprite* lightingSrcSprite;
    ClightingSourceSprite* lightingDstSprite;
    //core data of lighting
    CCPoint startPoint;//local space start point
    CCPoint endPoint;//local space end point
    vector<ClineSeg*> segList;//hold seges compose the lighting
    int nGeneration;//how many generation to split
    ClineSeg*headSeg;//the lighting tree's main branch's head seg
    ClineSeg*tailSeg;//the lighting tree's main branch's tail seg
    //model
    Cmesh mesh;
    CindexVBO indexVBO;
    CmeshWire mesh_wire;
    CindexVBO indexVBO_wire;
    //animation control
    float dOpacity;//opacity decrement per second
    //settings
    float k_offset;//maxOffset div segLen
    float k_offsetGrow;//k_offset should small, and k_offsetGrow bigger
    bool isDebugDraw;
    eJitterMode jitterMode;
    bool isDoJitter;
    float howManyJitterInOneFade;
    float percentageOfJitterBranch;
    float mainSegLen;
    bool isDrawBrightLine;
    float genBranchProbabilityFade;
};
#endif /* defined(__HelloCpp__lightingSprite__) */
