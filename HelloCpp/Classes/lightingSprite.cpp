

#include "lightingSprite.h"
CGLProgramWithMyUnifos ClightingSprite::program;
bool ClineSeg::addChildRef(ClineSeg*child){
    //if already have this child, do not add, return false
    int nChild=(int)childList.size();
    for(int i=0;i<nChild;i++){
        if(childList[i]==child){
            return false;
        }
    }
    childList.push_back(child);
    return true;
}
bool ClineSeg::removeChildRef(ClineSeg*child){
    //only remove the first matched child
    int nChild=(int)childList.size();
    for(int i=0;i<nChild;i++){
        if(childList[i]==child){
            childList.erase(childList.begin()+i);
            nChild--;
            i--;
            return true;
        }
    }
    return false;
}

bool ClightingSourceSprite::init(string texFileName){
    this->initWithFile(texFileName.c_str());
    
    return true;
    
}
void ClightingSprite::initProgram(){
    //init shader program (lazy init)
    if(program.getProgram()==NULL)
    {
        GLchar * fragSource = (GLchar*) CCString::createWithContentsOfFile(CCFileUtils::sharedFileUtils()->fullPathForFilename("texColorExtraAlpha.fsh").c_str())->getCString();
        CCGLProgram* pProgram = new CCGLProgram();
        pProgram->initWithVertexShaderByteArray(ccPositionTextureColor_vert, fragSource);
        //bind attribute
        pProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        pProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        pProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        //link  (must after bindAttribute)
        pProgram->link();
        //get cocos2d-x build-in uniforms
        pProgram->updateUniforms();
        //get my own uniforms
        map<string,GLint> myUnifoMap;
        myUnifoMap["myAlpha"] = glGetUniformLocation(pProgram->getProgram(),"myAlpha");
        //make program
        program.myUnifoMap=myUnifoMap;
        program.setProgram(pProgram);
        //program can be released
        pProgram->release();
        //check gl error
        CHECK_GL_ERROR_DEBUG();
        
    }
}
bool ClightingSprite::init(){
    this->CCSprite::initWithFile("lightingSeg.png");
  //  this->getTexture()->setAliasTexParameters();
    this->setAnchorPoint(ccp(0,0));
    ccBlendFunc blendFuncSeg={GL_SRC_ALPHA,GL_ONE};
    this->setBlendFunc(blendFuncSeg);
    //create indexVBO
    indexVBO.genBuffers();
    indexVBO_wire.genBuffers();
    //*this opacity
    this->setOpacity(0);
    //lightingSourceSprite
    //lighting source
    {
        lightingSrcSprite=new ClightingSourceSprite();
        lightingSrcSprite->autorelease();
        lightingSrcSprite->init("lightingSource.png");
        lightingSrcSprite->setPosition(ccp(0,0));
        this->addChild(lightingSrcSprite);
        lightingSrcSprite->setVisible(false);
    }
    //lighting dest
    {
        lightingDstSprite=new ClightingSourceSprite();
        lightingDstSprite->autorelease();
        lightingDstSprite->init("lightingSource.png");
        lightingDstSprite->setPosition(endPoint);
        this->addChild(lightingDstSprite);
        lightingDstSprite->setVisible(false);
    }
    CHECK_GL_ERROR_DEBUG();

 

    return true;
}
void ClightingSprite::updateNGeneration(){
    float lightingLen=2*ccpDistance(startPoint, endPoint)*sqrtf(0.25+k_offset*k_offset);
    int count=0;
    while(lightingLen>mainSegLen){
        lightingLen/=2;
        count++;
    }//count is the smallest number let mainSegLen*2^count not less than lightingLen
    nGeneration=count;
    //  CCLOG("nGeneration:%i",nGeneration);
}

vector<ClineSeg*> ClightingSprite::genSegesFromBranchSeg(ClineSeg*branchSeg,int nGeneration)
//input branchSeg and get subSeges, branchSeg itself will be deleted
{
    if(nGeneration<=0){
        return vector<ClineSeg*>();
        CCLOG("warning:nGeneration<=0!");
    }
    vector<ClineSeg*> segList;
    //use branchSeg as inital seg
    segList.push_back(branchSeg);//branchSeg will be deleted in the loop below
    //begin generations
    //becuase in the loop we will change (delete) branchSeg, so here we must save branchSeg's branchStartGenerationID
    //in order to the loop length not changed during the loop.
    int branchStartGenerationID=branchSeg->branchStartGenerationID;
    for(int i=branchStartGenerationID+1;i<branchStartGenerationID+1+nGeneration;i++){
        vector<ClineSeg*> tSegList;
        //split each seg in segList
        int nseg=(int)segList.size();
        for(int j=0;j<nseg;j++){
            ClineSeg*seg=segList[j];//cur seg
            vector<ClineSeg*> subseges=splitSeg(seg,i);//split seg, get sub seges
            //add sub seges to segList
            int nSubSeg=(int)subseges.size();
            for(int k=0;k<nSubSeg;k++){
                tSegList.push_back(subseges[k]);
            }
        }
        //clear segList
        for(int j=0;j<nseg;j++){
            delete segList[j];
            segList[j]=NULL;
        }
        //
        segList=tSegList;
    }
    return segList;
}
void ClightingSprite::genLighting(){
    genSeges();
    genMesh();
}
void ClightingSprite::clearSeges(){
    //clear segList
    int nSeg=(int)segList.size();
    for(int i=0;i<nSeg;i++){
        delete segList[i];
        segList[i]=NULL;
    }
    segList.clear();
}
void ClightingSprite::genSeges(){
    //clear segList
    clearSeges();
    //if nGeneration==0, namely lighting is too short
    if(nGeneration==0){
        ClineSeg* branchSeg=new ClineSeg(this->startPoint,this->endPoint);
        branchSeg->branchID=0;
        branchSeg->branchStartGenerationID=0;
        branchSeg->generationID=0;
        branchSeg->branchEndPoint=this->endPoint;
        segList.push_back(branchSeg);
        headSeg=branchSeg;
        tailSeg=branchSeg;
        return;
    }
    //create inital branchSeg
    ClineSeg* branchSeg=new ClineSeg(this->startPoint,this->endPoint);
    branchSeg->branchID=0;
    branchSeg->branchStartGenerationID=0;
    branchSeg->generationID=0;
    branchSeg->branchEndPoint=this->endPoint;
    //use inital branchSeg to create segList
    segList=genSegesFromBranchSeg(branchSeg, nGeneration);
    //find head and tail seg
    headSeg=segList[0];
    tailSeg=segList[(int)segList.size()-1];
    assert(headSeg->branchID==0&&headSeg->father==NULL);
    assert(tailSeg->branchID==0&&(int)tailSeg->childList.size()==0);
}

vector<ClineSeg*> ClightingSprite::splitSeg(ClineSeg*seg,int generationID)
{
   // CCLOG("seg->father branchID:%i",seg->branchID);
    
    //  *-------seg-------*
    //  *--seg0--*--seg1--*
    //
    vector<ClineSeg*> seges;
    {
    float maxOffset=seg->getLen()*k_offset*nPow(k_offsetGrow,seg->branchID);
    float offset=randomInRange(-maxOffset,maxOffset);
    CCPoint midPoint=seg->getMidPoint()+ccpMult(seg->getRightNormal(),offset);
    ClineSeg* seg0=new ClineSeg(seg->startPoint,midPoint);
    ClineSeg* seg1=new ClineSeg(midPoint,seg->endPoint);
    seg0->branchID=seg->branchID;//sub seg's branchID inherited from seg
    seg0->branchStartGenerationID=seg->branchStartGenerationID;
    seg0->branchEndPoint=seg->branchEndPoint;
    seg0->generationID=seg->generationID+1;
    seg1->branchID=seg->branchID;
    seg1->branchStartGenerationID=seg->branchStartGenerationID;
    seg1->branchEndPoint=seg->branchEndPoint;
    seg1->generationID=seg->generationID+1;
    //update links of seg0, seg1
    seg0->father=seg->father;
    seg0->addChildRef(seg1);
    seg1->father=seg0;
    seg1->childList=seg->childList;
    //update links of seg's neighbour
    if(seg->father){
        seg->father->addChildRef(seg0);
        seg->father->removeChildRef(seg);
    }
    for(int i=0;i<(int)seg->childList.size();i++){
        ClineSeg*child=seg->childList[i];
        child->father=seg1;
    }
    //add seg0,seg2(if has),seg1 to seges, notice the order, if we keep this order, we can guarantee segList[0] is headSeg and segList[(int)segList.size()-1] is tailSeg.
    //note, jitter will break this law
    //add seg0 to seges
    seges.push_back(seg0);
    //calculate branch
    float p=nPow(genBranchProbabilityFade,generationID-1);
    if(catchProbability(p))
    {
        float randomSmallAngle=randomInRange(-3,3);//-3,3
        float lengthScale=0.6*nPow(1.2, seg->branchID);//0.7//0.6
        CCPoint splitEnd=ccpMult(rotateVec(seg0->getVec(),randomSmallAngle), lengthScale)+midPoint;
        ClineSeg*seg2=new ClineSeg(midPoint,splitEnd);
        seg2->branchID=seg->branchID+1;//branch's branchID is seg's branchID+1
        seg2->branchStartGenerationID=generationID;
        seg2->branchEndPoint=seg2->endPoint;
        seg2->generationID=seg->generationID+1;
        //update links of seg2
        seg2->father=seg0;
        seg0->addChildRef(seg2);
        //add seg2 to seges
        seges.push_back(seg2);
    }
    //add seg1 to seges
    seges.push_back(seg1);
    }
    return seges;
}
void ClightingSprite::doJitter_reGenBranches(){
    
    //generate branchList
    vector<ClineSeg*> branchList;
    {
        ClineSeg*seg=headSeg;
        while(1){
            //seg must be mainBranch seg
            //see if seg's children have secondary branch
            ClineSeg*nextSeg=NULL;
            int nChild=(int)seg->childList.size();
            if(nChild>1){//have secondary branch
                for(int i=0;i<nChild;i++){
                    ClineSeg*_seg=seg->childList[i];
                    if(_seg->branchID==1){//_seg is secondary branch root seg
                        //make branchSeg based on _seg
                        branchList.push_back(_seg);
                    }else if(_seg->branchID==0){
                        nextSeg=_seg;
                    }
                }
            }else if(nChild==1){
                nextSeg=seg->childList[0];
            }
            //if have next seg then go on, else jump out
            if(nextSeg){
                seg=nextSeg;
            }else{
                break;
            }
        }//got secondaryBranchList
 
    }
    
    //mark branch to be regenerated
    {
        int nbranch=(int)branchList.size();
        for(int i=0;i<nbranch;i++){
            ClineSeg*branchSeg=branchList[i];
            float p=percentageOfJitterBranch;
            if(catchProbability(p)){
                branchSeg->needReGenerate=true;
            }else{
                branchSeg->needReGenerate=false;
            }
        }

     }

    //generate seeds
    //if a branch need not regenerate, it's seed is NULL
    vector<ClineSeg*> seedList((int)branchList.size());
    {
        int nSeed=(int)seedList.size();
        for(int i=0;i<nSeed;i++){
            if(branchList[i]->needReGenerate){
                ClineSeg*seed=new ClineSeg();
                *seed=*branchList[i];
                seed->endPoint=seed->branchEndPoint;
                seed->generationID=branchList[i]->branchStartGenerationID;
                seed->childList.clear();
                seedList[i]=seed;
            }else{
                seedList[i]=NULL;
            }
            
        }
 
    }
    
    //clear branches need to regenerate
    releaseNeedReGenBranch(branchList,segList);
    
    
    //generate seges for each seg in secondaryBranchList
    int nSeed=(int)seedList.size();
    for(int i=0;i<nSeed;i++){
        ClineSeg *seed=seedList[i];
        if(seed==NULL)continue;
  //      seed->endPoint=seed->startPoint+rotateVec(seed->getVec(),randomInRange(-5, 5));//-5,5// i think without this line is better
        //generate seges for branchSeg
        int nGeneration_seed=nGeneration-seed->branchStartGenerationID;//how many generations this seed will grow
        if(nGeneration_seed<=0)continue;//because nGeneration may changed, so nGeneration_seed may <= 0
        vector<ClineSeg*> tSegList=genSegesFromBranchSeg(seed, nGeneration_seed);
        assert(tSegList[0]->father);
        assert((int)tSegList[(int)tSegList.size()-1]->childList.size()==0);
        //add tSegList to segList
        int ntSeg=(int)tSegList.size();
        for(int j=0;j<ntSeg;j++){
            segList.push_back(tSegList[j]);
        }
        
    }

}
void ClightingSprite::releaseNeedReGenBranch(vector<ClineSeg*>&branchList,vector<ClineSeg*>&segList){
    int nbranch=(int)branchList.size();
    for(int i=0;i<nbranch;i++){
        ClineSeg*branchSeg=branchList[i];
        if(branchSeg->needReGenerate){
            //remove its reference from its father's childList
            branchSeg->father->removeChildRef(branchSeg);
            //release this whole branch
            releaseNeedReGenBranch_inn(branchSeg,segList);
        }
    }
    //remove all NULL elements in segList
    int nseg=(int)segList.size();
    for(int i=0;i<nseg;i++){
        if(segList[i]->needReGenerate){
            delete segList[i];
            segList[i]=NULL;
        }
    }
    
    vector<ClineSeg*> tSegList;
    for(int i=0;i<nseg;i++){
        if(segList[i]!=NULL){
            tSegList.push_back(segList[i]);
        }
    }
    segList=tSegList;
}
void ClightingSprite::releaseNeedReGenBranch_inn(ClineSeg*seg,vector<ClineSeg*>&segList){
    if(seg==NULL)return;
    seg->needReGenerate=true;
    //same process for children
    int nChild=(int)seg->childList.size();
    for(int i=0;i<nChild;i++){
        ClineSeg*_seg=seg->childList[i];
        releaseNeedReGenBranch_inn(_seg,segList);
    }
}
void ClightingSprite::doJitter(){
    switch (jitterMode) {
        case eJitterMode_noJitter:
            
            break;
        case eJitterMode_reGenBranches:
            doJitter_reGenBranches();
            break;
        case eJitterMode_adjuVert:
            break;
            
        default:
            break;
    }
}

void ClightingSprite::genMesh(){

    //make mesh
    {
        mesh.vlist.clear();
        mesh.IDtriList.clear();
        mesh.texCoordList.clear();
        int nSeg=(int)segList.size();
        for(int i=0;i<nSeg;i++){
            ClineSeg*seg=segList[i];
            //calculate quad for seg
            //      v0        v3
            //       * -------*
            //       |segW/2  |
            //   end <--------- start
            //       |        |
            //       *--------*
            //      v1        v2
            const CCPoint&start=seg->startPoint;
            const CCPoint&end=seg->endPoint;
            CCPoint rightNorm=seg->getRightNormal();
            float segW;
      //      if(seg->branchID==0){
                segW=11*nPow(0.2,seg->branchID);//7,9 | 3.5
       //     }else{
        //        segW=7*nPow(0.87,seg->generationID);
        //    }
            
            CCPoint p0=end+ccpMult(rightNorm, segW/2);
            CCPoint p1=end+ccpMult(rightNorm, -segW/2);
            CCPoint p2=start+ccpMult(rightNorm, -segW/2);
            CCPoint p3=start+ccpMult(rightNorm, segW/2);
            {
                CCPoint mid=seg->getMidPoint();
                p0=mid+ccpMult(p0-mid, 8);
                p1=mid+ccpMult(p1-mid, 8);
                p2=mid+ccpMult(p2-mid, 8);
                p3=mid+ccpMult(p3-mid, 8);
                
            }
            CCPoint texCoord0=CCPoint(0,0);
            CCPoint texCoord1=CCPoint(0,1);
            CCPoint texCoord2=CCPoint(1,1);
            CCPoint texCoord3=CCPoint(1,0);
            mesh.vlist.push_back(Cv2(p0.x,p0.y));int vID0=(int)mesh.vlist.size()-1;
            mesh.vlist.push_back(Cv2(p1.x,p1.y));int vID1=(int)mesh.vlist.size()-1;
            mesh.vlist.push_back(Cv2(p2.x,p2.y));int vID2=(int)mesh.vlist.size()-1;
            mesh.vlist.push_back(Cv2(p3.x,p3.y));int vID3=(int)mesh.vlist.size()-1;
            mesh.texCoordList.push_back(Cv2(texCoord0.x,texCoord0.y));
            mesh.texCoordList.push_back(Cv2(texCoord1.x,texCoord1.y));
            mesh.texCoordList.push_back(Cv2(texCoord2.x,texCoord2.y));
            mesh.texCoordList.push_back(Cv2(texCoord3.x,texCoord3.y));
            CIDTriangle IDtri0(vID0,vID1,vID2);
            CIDTriangle IDtri1(vID0,vID2,vID3);
            mesh.IDtriList.push_back(IDtri0);
            mesh.IDtriList.push_back(IDtri1);

        }
        
    }
    //submit mesh to indexVBO
    {
        //submit data to VBO
        indexVBO.submitPos(mesh.vlist,GL_STREAM_DRAW);//GL_DYNAMIC_DRAW
        indexVBO.submitTexCoord(mesh.texCoordList,GL_STREAM_DRAW);
        indexVBO.submitIndex(mesh.IDtriList,GL_STREAM_DRAW);
        //check gl error
        CHECK_GL_ERROR_DEBUG();
    }
    
    if(isDrawBrightLine){
        //make mesh_wire
        {
            mesh_wire.vlist.clear();
            mesh_wire.IDlineList.clear();
            mesh_wire.texCoordList.clear();
            int nSeg=(int)segList.size();
            for(int i=0;i<nSeg;i++){
                ClineSeg*seg=segList[i];
                const CCPoint&start=seg->startPoint;
                const CCPoint&end=seg->endPoint;
                mesh_wire.vlist.push_back(Cv2(start.x,start.y));int vID0=(int)mesh_wire.vlist.size()-1;
                mesh_wire.vlist.push_back(Cv2(end.x, end.y));int vID1=(int)mesh_wire.vlist.size()-1;
                mesh_wire.texCoordList.push_back(Cv2(0.5,0.5));
                mesh_wire.texCoordList.push_back(Cv2(0.5,0.5));
                CIDLine IDline(vID0,vID1);
                mesh_wire.IDlineList.push_back(IDline);
                
            }
            
        }
        //submit mesh_wire to indexVBO_wire
        {
            //submit data to VBO
            indexVBO_wire.submitPos(mesh_wire.vlist,GL_STREAM_DRAW);//GL_DYNAMIC_DRAW
            indexVBO_wire.submitTexCoord(mesh_wire.texCoordList,GL_STREAM_DRAW);
            indexVBO_wire.submitIndex(mesh_wire.IDlineList,GL_STREAM_DRAW);
            //check gl error
            CHECK_GL_ERROR_DEBUG();
        }
    }
    
  

    
    
}
void ClightingSprite::setStartPoint(CCPoint startPointWorld){
    CCPoint _startPoint=this->convertToNodeSpace(startPointWorld);
    
    //if startPoint not changed, return
    if(pointEqual(startPoint, _startPoint, 0.001)){
        return;
    }else{
        startPoint=_startPoint;
    }
    //startPoint changed

    //relocate lightSourceSprite
    lightingSrcSprite->setPosition(_startPoint);
    //relocate all the seges in segList
    if(headSeg&&tailSeg)
    {
        //rotate all seges in segList
        CCPoint vec_TailEndToHeadStart=headSeg->startPoint-tailSeg->endPoint;
        CCPoint vec_TailEndToStart=this->startPoint-tailSeg->endPoint;
        CCPoint cosA_sinA=calculateAngleFromVec1ToVec2(vec_TailEndToHeadStart, vec_TailEndToStart);
        float cosA=cosA_sinA.x;
        float sinA=cosA_sinA.y;
        int nseg=(int)segList.size();
        for(int i=0;i<nseg;i++){
            ClineSeg*seg=segList[i];
            seg->startPoint=tailSeg->endPoint+rotateVec(seg->startPoint-tailSeg->endPoint, cosA,sinA);
            seg->endPoint=tailSeg->endPoint+rotateVec(seg->endPoint-tailSeg->endPoint, cosA,sinA);
            if(seg->branchID==1&&seg->father->branchID==0){//secondary branch root seg
                seg->branchEndPoint=tailSeg->endPoint+rotateVec(seg->branchEndPoint-tailSeg->endPoint, cosA, sinA);
            }
        }
        //scale segList, use headSeg->startPoint as the center
        float dis_TailEndToStart=ccpDistance(tailSeg->endPoint, this->startPoint);
        float dis_TailEndToHeadStart=ccpDistance(tailSeg->endPoint, headSeg->startPoint);
        float scaleFactor=dis_TailEndToStart/dis_TailEndToHeadStart;
        for(int i=0;i<nseg;i++){
            ClineSeg*seg=segList[i];
            seg->startPoint=tailSeg->endPoint+ccpMult(seg->startPoint-tailSeg->endPoint, scaleFactor);
            seg->endPoint=tailSeg->endPoint+ccpMult(seg->endPoint-tailSeg->endPoint, scaleFactor);
            if(seg->branchID==1&&seg->father->branchID==0){//secondary branch root seg
                seg->branchEndPoint=tailSeg->endPoint+ccpMult(seg->branchEndPoint-tailSeg->endPoint, scaleFactor);
            }
        }
        //because segList changed, so must re-generate mesh and glow sprites
        genMesh();
        
        
        
    }
    //update nGeneration
    updateNGeneration();

}
void ClightingSprite::setEndPoint(CCPoint endPointWorld){
    
    
    CCPoint _endPoint=this->convertToNodeSpace(endPointWorld);
    
    //if endPoint not changed, return
    if(pointEqual(endPoint, _endPoint, 0.001)){
        return;
    }else{
        endPoint=_endPoint;
    }
    //endPoint changed
  
    //relocate lightSourceSprite2
    lightingDstSprite->setPosition(endPoint);
    //relocate all the seges in segList
    if(headSeg&&tailSeg)
    {
        //rotate all seges in segList
        CCPoint vec_headStartToTailEnd=tailSeg->endPoint-headSeg->startPoint;
        CCPoint vec_headStartToEnd=this->endPoint-headSeg->startPoint;
        CCPoint cosA_sinA=calculateAngleFromVec1ToVec2(vec_headStartToTailEnd, vec_headStartToEnd);
        float cosA=cosA_sinA.x;
        float sinA=cosA_sinA.y;
        int nseg=(int)segList.size();
        for(int i=0;i<nseg;i++){
            ClineSeg*seg=segList[i];
            seg->startPoint=headSeg->startPoint+rotateVec(seg->startPoint-headSeg->startPoint, cosA,sinA);
            seg->endPoint=headSeg->startPoint+rotateVec(seg->endPoint-headSeg->startPoint, cosA,sinA);
            if(seg->branchID==1&&seg->father->branchID==0){//secondary branch root seg
                seg->branchEndPoint=headSeg->startPoint+rotateVec(seg->branchEndPoint-headSeg->startPoint, cosA, sinA);
            }
        }
        //scale segList, use headSeg->startPoint as the center
        float dis_headStartToEnd=ccpDistance(headSeg->startPoint, this->endPoint);
        float dis_headStartToTailEnd=ccpDistance(headSeg->startPoint, tailSeg->endPoint);
        float scaleFactor=dis_headStartToEnd/dis_headStartToTailEnd;
        for(int i=0;i<nseg;i++){
            ClineSeg*seg=segList[i];
            seg->startPoint=headSeg->startPoint+ccpMult(seg->startPoint-headSeg->startPoint, scaleFactor);
            seg->endPoint=headSeg->startPoint+ccpMult(seg->endPoint-headSeg->startPoint, scaleFactor);
            if(seg->branchID==1&&seg->father->branchID==0){//secondary branch root seg
                seg->branchEndPoint=headSeg->startPoint+ccpMult(seg->branchEndPoint-headSeg->startPoint, scaleFactor);
            }
        }
        //because segList changed, so must re-generate mesh and glow sprites
        genMesh();
        
        
        
    }
    //update nGeneration
    updateNGeneration();
    
}
bool ClightingSprite::isFadeOut() {
    return (this->getOpacity()-dOpacity*minDeltaTime<=0);//if sub minDeltaTime less than zero, sub realy deltaTime must less than zero
}
void ClightingSprite::update(float t){
    //t is the deltaTime between previous frame and crrent frame.
    //t must be >= minDeltaTime
    //here we use t to calculate update to make animation independent of frame rate

   // CCLOG("t:%f",t);
   // CCLOG("nGeneration:%i",nGeneration);
    this->setOpacity(this->getOpacity()-dOpacity*t);
    if(isFadeOut()){
        genLighting();
    }
    
    float step=255.0/(howManyJitterInOneFade+1);
    for(int i=0;i<howManyJitterInOneFade;i++){
        float value=(i+1)*step;
        if(this->getOpacity()>=value&&this->getOpacity()-dOpacity*minDeltaTime<value){
            doJitter();
            genMesh();
        }
    }
}
void ClightingSprite::debugDraw_segWireFrame(float lineWidth){
    glLineWidth(lineWidth);
    //draw mesh wireframe
    int nIDtri=(int)mesh.IDtriList.size();
    for(int i=0;i<nIDtri;i++){
        CIDTriangle&IDtri=mesh.IDtriList[i];
        Cv2&p0=mesh.vlist[IDtri.vID[0]];
        Cv2&p1=mesh.vlist[IDtri.vID[1]];
        Cv2&p2=mesh.vlist[IDtri.vID[2]];
        CCPoint vertices[3]={ccp(p0.v[0],p0.v[1]),ccp(p1.v[0],p1.v[1]),ccp(p2.v[0],p2.v[1])};
        ccDrawPoly(vertices, 3, true);
    }

}
void ClightingSprite::debugDraw_segLineAndDot(float lineWidth,float pointSize){
     //draw segList
     glLineWidth(lineWidth);
     ccPointSize(pointSize);
     int nSeg=(int)segList.size();
     for(int i=0;i<nSeg;i++){
         ClineSeg*seg=segList[i];
         ccDrawLine(seg->startPoint, seg->endPoint);
         ccDrawPoint(seg->startPoint);
         ccDrawPoint(seg->endPoint);
     }
}
void ClightingSprite::draw(){
    //draw mesh
    //----change shader
    ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );
    this->setShaderProgram(program.getProgram());
    ccGLEnable(m_eGLServerState);
    //pass values for cocos2d-x build-in uniforms
    getShaderProgram()->use();
    getShaderProgram()->setUniformsForBuiltins();
    //pass values for my own uniforms
    float alpha=this->getOpacity()*1.0/255*3.1;
    glUniform1f(program.myUnifoMap["myAlpha"],alpha);
    //----enable attribute arrays (must do every frame)
    CindexVBO::enableAttribArrays();
    //ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );
    //----bindTexture
    ccGLBindTexture2D( this->getTexture()->getName());//do not use glBindTexture2D, or next sprite use cocos2d-x default draw may be affected
    //----draw
    indexVBO.setPointers();
    indexVBO.draw(GL_TRIANGLES);
    //----draw
    if(isDrawBrightLine){
        float alpha_line=this->getOpacity()*1.0/255;
        glUniform1f(program.myUnifoMap["myAlpha"],alpha_line);
        glLineWidth(0.5);//here use glLineWidth is no problem
        indexVBO_wire.setPointers();
        indexVBO_wire.draw(GL_LINES);
    }
    //----unbindTexture
    ccGLBindTexture2D(0);//do not use glBindTexture2D, or next sprite use cocos2d-x default draw may be affected
   
    //debugDraw
    if(isDebugDraw){
        debugDraw_segLineAndDot(0.5,3);
        debugDraw_segWireFrame(1);
    }
    //CHECK_GL_ERROR_DEBUG();
}