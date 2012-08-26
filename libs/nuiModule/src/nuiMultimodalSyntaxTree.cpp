/////////////////////////////////////////////////////////////////////////////
// Name:        containers/ccfMultimodalSyntaxTree.cpp
// Purpose:     Multimodal syntax tree implementation
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiMultimodalSyntaxTree.h"
  
int const tabsize = 4;

std::string tab_s(int indent)
{
    std::ostringstream out;
    for (int i = 0; i < indent; ++i) {
        out << std::string(" ");
    }
    return out.str();
}

struct mast_printer_s
{
    mast_printer_s(int indent = 0) : indent(indent) {}
    
    std::string operator()(client::multimodalSyntaxTree const& mast) const;
    
    int indent;
};

struct mast_node_printer_s : boost::static_visitor<std::string>
{
    mast_node_printer_s(int indent = 0) : indent(indent) {}
    
    std::string operator()(client::multimodalSyntaxTree const& mast) const {
        return mast_printer_s(indent+tabsize)(mast);
    }
    
    std::string operator()(client::multimodalLeafNode const& mmNode) const {
        std::ostringstream out;
        out << tab_s(indent+tabsize);
        out << "|multimodal| type: \"" << mmNode.type << "\" |\n";
        return out.str();
    }
    
    std::string operator()(client::unimodalLeafNode const& umNode) const {
        std::ostringstream out;
        out << tab_s(indent+tabsize);
        out << "|unimodal| type: \"" << umNode.type << "\", val: \"" << umNode.val << "\" |\n";
        return out.str();
    }
    
    int indent;
};

std::string mast_printer_s::operator()(client::multimodalSyntaxTree const& mast) const
{
    std::ostringstream out;
    out << tab_s(indent);
    out << "|abstract| type: \"" << mast.type << "\", val: \"" << mast.val << "\", children.size(): " << mast.children.size() << " |\n";
    out << tab_s(indent);
    out << '{' << std::endl;
    BOOST_FOREACH(client::node const& nod, mast.children)
    {
        out << boost::apply_visitor(mast_node_printer_s(indent), nod);
    }
    out << tab_s(indent);
    out << '}' << std::endl;
    return out.str();
}

std::string mast_to_string(client::multimodalSyntaxTree *mast) {
    mast_printer_s printer;
    return printer(*mast);
}

struct mast_printer_json
{    
    cJSON* operator()(client::multimodalSyntaxTree const& mast) const;
};

struct mast_node_printer_json : boost::static_visitor<cJSON *>
{    
    cJSON* operator()(client::multimodalSyntaxTree const& mast) const {
        return mast_printer_json()(mast);
    }
    
    cJSON* operator()(client::multimodalLeafNode const& mmNode) const {
        cJSON* out = cJSON_CreateObject();
        cJSON_AddStringToObject(out, "type", mmNode.type.c_str());
        return out;
    }
    
    cJSON* operator()(client::unimodalLeafNode const& umNode) const {
        cJSON* out = cJSON_CreateObject();
        cJSON_AddStringToObject(out, "type", umNode.type.c_str());
        cJSON_AddStringToObject(out, "val", umNode.val.c_str());
        return out;
    }
};

cJSON* mast_printer_json::operator()(client::multimodalSyntaxTree const& mast) const
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "type", mast.type.c_str());
    cJSON_AddStringToObject(root, "val", mast.val.c_str());
    cJSON *children = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "children", children);
    BOOST_FOREACH(client::node const& nod, mast.children)
    {
        cJSON_AddItemToArray(children, boost::apply_visitor(mast_node_printer_json(), nod));
    }
    return root;
}

cJSON* mast_to_json(client::multimodalSyntaxTree *mast) {
    mast_printer_json printer;
    return printer(*mast);
}
