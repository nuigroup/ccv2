/////////////////////////////////////////////////////////////////////////////
// Name:        models/ballWorldGrammar.cpp
// Purpose:     BallWorld grammar for the Spirit parser module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "ballWorldGrammar.h"

namespace client {
        
    template <typename Iterator> struct spiritGrammar : qi::grammar<Iterator, multimodalSyntaxTree(), ascii::space_type>
	{
        spiritGrammar() : spiritGrammar::base_type(interaction)
        {
            
			using qi::lit;
			using qi::attr;
			
			using ascii::string;
			using namespace qi::labels;
            
            
            interaction %= (moveInteraction | colorInteraction | createInteraction | destroyInteraction); // type is name of the rule, val is string, and children is children
            
            moveInteraction %= moveCommand >> attr("interaction") >> movePredicate;
            colorInteraction %= colorCommand >> attr("interaction") >> colorPredicate;
            createInteraction %= createCommand >> attr("interaction") >> createPredicate;
            destroyInteraction %= destroyCommand >> attr("interaction") >> destroyPredicate;
            
            movePredicate %= (ball >> point);
            colorPredicate %= (ball >> color);
            createPredicate %= (newBall >> point);
            destroyPredicate %= (ball);
            
            
            moveCommand = (lit("move") | lit("put")) [_val = "move"]; // val should be tuple<string type, string command>
            colorCommand = lit("color") [_val = "color"];
            destroyCommand = (lit("delete") | lit("remove") | lit("destroy")) [_val = "destroy"];
            createCommand = (lit("make") | lit("create")) [_val = "create"];
            
            ball %= ballSelectorUnimodal | ballSelectorMultimodal; // pass on the ball node
            ballSelectorUnimodal %= lit("the") >> colorSelectorUnimodal  >> (lit("ball") | lit("one"));
            ballSelectorMultimodal %= (lit("this") | lit("that")) >> attr("ball") >> -(lit("ball") | lit("one"));
            
            point %= pointSelectorMultimodal; // build a point object
            pointSelectorMultimodal %= (lit("here") | lit("there")) >> attr("point");
            
            newBall %= (lit("an") | lit("a")) >> -lit("new") >> color >> (lit("ball") | lit("one")); // ball id needs replacing
            
            color %= colorSelectorUnimodal; // build a color object (or its data), name of the rule is the type attribute and val is the returned attribute
            
            colorSelectorUnimodal %= (string("red") | string("green") | string("blue") | string("yellow") | string("orange") | string("white") | string("purple")) >> attr("color"); // return a unimodal leaf, this is terminal
            
            
            
            
        }
        
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> interaction;
        
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> moveInteraction;
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> colorInteraction;
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> createInteraction;
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> destroyInteraction;
        
        qi::rule<Iterator, std::vector<node>(), ascii::space_type> createPredicate;
        qi::rule<Iterator, std::vector<node>(), ascii::space_type> colorPredicate;
        qi::rule<Iterator, std::vector<node>(), ascii::space_type> movePredicate;
        qi::rule<Iterator, std::vector<node>(), ascii::space_type> destroyPredicate;
        
		
        qi::rule<Iterator, std::string(), ascii::space_type> moveCommand;
        qi::rule<Iterator, std::string(), ascii::space_type> colorCommand;
        qi::rule<Iterator, std::string(), ascii::space_type> createCommand;
        qi::rule<Iterator, std::string(), ascii::space_type> destroyCommand;
        
        qi::rule<Iterator, node(), ascii::space_type> ball;
        qi::rule<Iterator, multimodalLeafNode(), ascii::space_type> ballSelectorMultimodal;
        qi::rule<Iterator, unimodalLeafNode(), ascii::space_type> ballSelectorUnimodal;
        
        qi::rule<Iterator, node(), ascii::space_type> newBall;
        
		qi::rule<Iterator, node(), ascii::space_type> point;
        qi::rule<Iterator, multimodalLeafNode(), ascii::space_type> pointSelectorMultimodal;
        
        qi::rule<Iterator, node(), ascii::space_type> color;
        qi::rule<Iterator, unimodalLeafNode(), ascii::space_type> colorSelectorUnimodal;
        
        
    };
    
    // somewhere in here we need a phoenix::ref to a counter for multimodal sequence
    
}

