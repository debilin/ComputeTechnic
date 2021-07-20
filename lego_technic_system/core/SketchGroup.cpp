//
// Created by 徐豪 on 2018/4/23.
//

#include "util/util.h"
#include <fstream>
#include "configure/global_variables.h"
#include "../core/LayerGraph.h"
#include "../core/SketchGroup.h"

bool SketchGroup::connect(SketchGroup* group)
{
    for(auto& line : this->lines){
        for(auto& line2 : group->lines){
            if(line->isConnected(line2))
                return true;
        }
    }
    return false;
}

int SketchGroup::getOrientationTag(){
    assert(isStraightLines(this->lines));
    assert(!this->lines.empty());
    SketchLine* line = *this->lines.begin();
    auto direction = line->getVector();
    Vector3d z_vec = Vector3d(0,0,1);
    Vector3d y_vec = Vector3d(0,1,0);
    Vector3d x_vec = Vector3d(1,0,0);

    if(fabs(fabs( (direction.dot(z_vec) / (direction.norm()*z_vec.norm()))) - 1) < 0.03){
        return Constants::PLANE_YZ_OR_XZ;
    }else if(fabs(fabs( (direction.dot(y_vec) / (direction.norm()*y_vec.norm()))) - 1) < 0.03){
        return Constants::PLANE_XY_OR_YZ;
    }else if(fabs(fabs( (direction.dot(x_vec) / (direction.norm()*x_vec.norm()))) - 1) < 0.03){
        return Constants::PLANE_XY_OR_XZ;
    } else{
        printf("Line direction error!!! Not an orthorgnal line\n");
        cin.get();
        exit(1);
    }
}

void SketchGroup::writeSketchToObj(vector<SketchPoint*> m_points, std::string remark){
    using std::ofstream;
    ofstream fout;
    string layer_tag = "sketch_decompse";
    std::ostringstream stringStream;
    stringStream << glob_vars::current_model.name <<"_"  << getCurrentTimeAsString()<<"_"<<remark<<".obj";
    std::string file_name = stringStream.str();
    string file_path = glob_vars::PRE_FIX + glob_vars::current_model.getDirectoryName() + "/generate/0_plane/" + file_name;

    fout.open(file_path);
    fout<<"# Auto Generated by Program \n";
    for(int i = 0;i<m_points.size();i++)
    {
        SketchPoint* p = m_points.at(i);
        fout<<"v "<< p->_x<<" "<<p->_y<<" "<<p->_z<<"\n";
    }
    fout<<"g Spine\n";
    int line_count = 0;
    for(auto& line : this->lines){
        SketchPoint *p1 = line->_p1;
        SketchPoint *p2 = line->_p2;
        fout<<"g Segment"<<line_count++<<"\n";
        fout<<"l "<<p1->_id+1<<" "<<p2->_id+1<<"\n"; //here +1 because the index in obj file start from 1, but our program start from 0
    }

    std::cout<<"File \""<<file_path<<"\" generated\n";
    fout.close();
}

int SketchGroup::totalSketchLength(){
    double length = 0.0;
    for(auto& l : lines){
        length += l->length();
    }
    return  (int)round(length);
}

set<int> SketchGroup::symmetryDirections(SketchGroup *group)//return whether this two groups are symmetry
{
    set<int> symmetryDirections{Constants::X_AXIS, Constants::Y_AXIS, Constants::Z_AXIS};
    if(this->orientation_tag != group->orientation_tag || this->lines.size()!=group->lines.size())
        return set<int>();
    for(auto& line1 : this->lines){
        set<int> symm_direction_line1;
        for(auto& line2 : group->lines){
            auto lineSymmDirections = line1->symmetryDirections(line2);
            if(!lineSymmDirections.empty()){
                symm_direction_line1 = lineSymmDirections;
                break;
            }
        }
        if(symm_direction_line1.empty())
            return symm_direction_line1;
        else{
            set<int> result;
            std::set_intersection(symmetryDirections.begin(), symmetryDirections.end(),
                                  symm_direction_line1.begin(),symm_direction_line1.end(),
                                  std::inserter(result,result.end()));
            symmetryDirections = result;
        }
    }
    return symmetryDirections;
}

bool SketchGroup::isConnectorComponent()
{
    if(this->lines.size() == 1 && (*this->lines.begin())->intLength()<=2)
        return false;
    if( this->orientation_tag == Constants::PLANE_YZ_OR_XZ ||
        this->orientation_tag == Constants::PLANE_XY_OR_YZ||
        this->orientation_tag == Constants::PLANE_XY_OR_XZ)
        return true;
    return false;
}

std::tuple<int,int,double,double> SketchGroup::getPossibleOriensAndCoord(){
    //assert(this->isConnectorComponent());
    double x_coord = (*this->lines.begin())->_p1->_x;
    double y_coord = (*this->lines.begin())->_p1->_y;
    double z_coord = (*this->lines.begin())->_p1->_z;
    switch(this->orientation_tag){
        case Constants::PLANE_XY_OR_XZ:{
            return std::make_tuple(Constants::PLANE_XY, Constants::PLANE_XZ,z_coord,y_coord);
            break;
        }
        case Constants::PLANE_XY_OR_YZ:{
            return std::make_tuple(Constants::PLANE_XY, Constants::PLANE_YZ,z_coord,x_coord);
            break;
       }
        case Constants::PLANE_YZ_OR_XZ:{
            return std::make_tuple(Constants::PLANE_YZ, Constants::PLANE_XZ, x_coord,y_coord);
            break;
        }
        default:{
            printf("Layer classification error\n");
            cin.get();
            exit(1);
        }
    }
}

double SketchGroup::getLayerCoordinate(){
    assert(!this->isConnectorComponent());
    switch(this->orientation_tag){
        case Constants::PLANE_XZ:{
            return (*this->lines.begin())->_p1->_y;
            break;
        }
        case Constants::PLANE_YZ:{
            return (*this->lines.begin())->_p1->_x;
            break;
        }
        case Constants::PLANE_XY:{
            return (*this->lines.begin())->_p1->_z;
            break;
        }
        default:{
            printf("Layer classification error\n");
            cin.get();
            exit(1);
        }
    }
}

void SketchGroup::assignOrientInfo(){
    if(this->orientation_tag == Constants::PLANE_XY ||
       this->orientation_tag == Constants::PLANE_XZ ||
       this->orientation_tag == Constants::PLANE_YZ)
    {
        for(auto& l : this->lines)
            l->chosen_orientation = this->orientation_tag;
    }
    else if(this->isConnectorComponent())
    {
        std::tuple<int,int,double,double> possi_orients = this->getPossibleOriensAndCoord();
        for(auto& l : this->lines)
            l->chosen_orientation = std::get<0>(possi_orients);
    }else{
        printf("Orientation assign error!!!\n");
    }
}

bool SketchGroup::isSelfSymmetry(){
    if(this->orientation_tag == Constants::PLANE_XZ){
        if(fabs((*this->lines.begin())->_p1->_y)<0.05)
            return true;
    }
    return false;
}




void DFS_symmetry_groups(SketchGroup* g, set<SketchGroup*>& result){
    result.insert(g);
    set<SketchGroup*> adj_groups;
    if(g->symm_group_x != NULL) adj_groups.insert(g->symm_group_x);
    if(g->symm_group_y != NULL) adj_groups.insert(g->symm_group_y);
    if(g->symm_group_z != NULL) adj_groups.insert(g->symm_group_z);
    for(auto& e : adj_groups){
        if(result.find(e) == result.end())//1.需要在这个component上 2)rigid 3)当前数据集中还没有
            DFS_symmetry_groups(e,result);
    }
}


vector<SketchGroup*> SketchGroup::getAllSymmetryGroups(){
    set<SketchGroup*> result;
    DFS_symmetry_groups(this,result);
    vector<SketchGroup*> result_vec(result.begin(), result.end());
    return result_vec;
}

void DFS_symmetry_groups_with_dirc( tuple<SketchGroup*, vector<int>> & g, set<tuple<SketchGroup*, vector<int>>>& result, SketchGroup* current_group){

    if(std::get<0>(g) != current_group){
        result.insert(g);
    }
    vector<int> last_direction = std::get<1>(g);
    set< tuple<SketchGroup*,vector<int>> > adj_groups;

    if(std::get<0>(g)->symm_group_x != NULL){
        vector<int> current_direc = last_direction;
        current_direc.push_back(Constants::X_AXIS);
        adj_groups.insert(std::make_tuple(std::get<0>(g)->symm_group_x, current_direc));
    }
    if(std::get<0>(g)->symm_group_y != NULL){
        vector<int> current_direc = last_direction;
        current_direc.push_back(Constants::Y_AXIS);
        adj_groups.insert(std::make_tuple(std::get<0>(g)->symm_group_y, current_direc));
    }
    if(std::get<0>(g)->symm_group_z != NULL){
        vector<int> current_direc = last_direction;
        current_direc.push_back(Constants::Z_AXIS);
        adj_groups.insert(std::make_tuple(std::get<0>(g)->symm_group_z, current_direc));
    }
    for(auto e : adj_groups){
        if(std::find_if(result.begin(),result.end(), [&e](const tuple<SketchGroup*,vector<int>>& tuple1)
        { return std::get<0>(tuple1) == std::get<0>(e); })
           == result.end() && std::get<0>(e) != current_group)
            DFS_symmetry_groups_with_dirc(e,result,current_group);
    }
}

set<tuple<SketchGroup*, vector<int>>> SketchGroup::getAllSymmetryGroupsWithDirecTag(){
    set<tuple<SketchGroup*,vector<int>>> result;
    tuple<SketchGroup*,vector<int>> this_tuple = std::make_tuple(this,vector<int>());
    DFS_symmetry_groups_with_dirc(this_tuple, result, this);
    return result;
}








