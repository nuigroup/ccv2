/////////////////////////////////////////////////////////////////////////////
// Name:        models/ballWorldGrammar.h
// Purpose:     BallWorld grammar for the Spirit parser module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef _ballWorldGrammar_h
#define _ballWorldGrammar_h

#include "nuiMultimodalSyntaxTree.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

namespace client {
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
}


BOOST_FUSION_ADAPT_STRUCT(
                          client::multimodalSyntaxTree,
                          (std::string, val)
                          (std::string, type)
                          (std::vector<client::node>, children)
                          )

BOOST_FUSION_ADAPT_STRUCT(
                          client::multimodalLeafNode,
                          (std::string, type)
                          )

BOOST_FUSION_ADAPT_STRUCT(
                          client::unimodalLeafNode,
                          (std::string, val)
                          (std::string, type)
                          )


#endif
