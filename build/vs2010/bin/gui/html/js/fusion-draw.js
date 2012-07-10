//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2010 Movid Authors.  All rights reserved.                  //
//                                                                          //
// This file is part of the Movid Software.                                 //
//                                                                          //
// This file may be distributed under the terms of the Q Public License     //
// as defined by Trolltech AS of Norway and appearing in the file           //
// LICENSE included in the packaging of this file.                          //
//                                                                          //
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE  //
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.//
//                                                                          //
// Contact info@movid.org if any conditions of this licensing are           //
// not clear to you.                                                        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

//
// declarations
//

var cfg_fontsize = 11;
var cfg_screen_w = 800;
var cfg_screen_h = 600;

var selected_widget = -1;
var selected_widget_x = 0;
var selected_widget_y = 0;
var selected_input_widget;
var selected_input_idx = -1;
var selected_output_widget;
var selected_output_idx = -1;
var connectivity_widget;
var connectivity_io;
var connectivity_idx;
var connectivity_ok = 0;
var config_show = 0;
var oldmouseX = 0;
var oldmouseY = 0;
var color_ok = null;
var color_ko = null;
var color_idle = null;
var color_selected = null;
var widgets_hover;
var widgets;

//
// Class that represent a connectivity of IO of a widget
//
var IOWidget = $.buildClass({
	constructor: function(_name, _type) {
		this.name = _name;
		this.type = _type;
		this.observers = pjs.ArrayList();
		this.observers_idx = pjs.ArrayList();
	},
	
	connect: function(_w, _idx) {
		this.observers.add(_w);
		this.observers_idx.add(_idx);
	},

	clearConnectivity: function() {
		for ( var i = 0; i < this.observers.size(); i++ ) {
			this.observers.remove(this.observers.get(0));
			this.observers_idx.remove(this.observers_idx.get(0));
		}
	}
});

//
// Class that represent a Widget
//
var Widget = $.buildClass({
	constructor: function() {
		this.x = 50;
		this.y = 50;
		this.w = 120;
		this.h = 75;
		this.name = 'UnknownID';
		this.positionChanged = true;
		this.gui = 0;
		this.inputs = pjs.ArrayList();
		this.outputs = pjs.ArrayList();
	},

	getInputCount: function() {
		return this.inputs.size();
	},

	getOutputCount: function() {
		return this.outputs.size();
	},

	getInput: function(idx) {
		return this.inputs[idx];
	},

	getOutput: function(idx) {
		return this.outputs[idx];
	},

	collide: function(_x, _y) {
		return _x >= this.x && _x <= this.x + this.w && _y >= this.y && _y <= this.y + this.h;
	},

	collideInput: function(_x, _y) {
		_h = this.headerSize();
		if ( _x < this.x || _x > this.x + 20 )
			return -1;
		if ( _y < _h || _y >= _h + this.inputs.size() * 20 )
			return -1;
		_y -= _h;
		_y = _y / 20;
		idx = Math.floor(_y);
		if ( _y - idx <= 0.75 )
			return idx;
		return -1;
	},

	collideOutput: function(_x, _y) {
		_h = this.headerSize();
		if ( _x < this.x + this.w - 20 || _x > this.x + this.w )
			return -1;
		if ( _y < _h || _y >= _h + this.outputs.size() * 20 )
			return -1;
		_y -= _h;
		_y = _y / 20;
		idx = Math.floor(_y);
		if ( _y - idx <= 0.75 )
			return idx;
		return -1;
	},

	draw: function() {
		this.drawBox();
		this.drawInputs();
		this.drawOutputs();
	},

	drawInputs: function() {

		_x = this.x;
		_y = this.headerSize();
		for ( var i = 0; i < this.inputs.size(); i++ ) {
			pjs.fill(50, 150, 250);

			for ( var j = 0; j < widgets_hover.size(); j++ ) {
				o = widgets_hover[j];
				if ( this != o[0] || o[1] != 'input' || o[2] != i )
					continue;
				pjs.fill(0x00, 0xea, 0x8b);
				break;
			}

			pjs.rect(_x, _y, 15, 15);
			_y += 20;
		}
	},

	drawOutputs: function() {

		_x = this.x + this.w - 15;
		_y = this.headerSize();
		for ( var i = 0; i < this.outputs.size(); i++ ) {
			pjs.fill(50, 150, 250);

			for ( var j = 0; j < widgets_hover.size(); j++ ) {
				o = widgets_hover[j];
				if ( this != o[0] || o[1] != 'output' || o[2] != i )
					continue;
				pjs.fill(0x00, 0xea, 0x8b);
				break;
			}

			pjs.rect(_x, _y, 15, 15);
			_y += 20;
		}
	},

	drawBox: function() {
		var do_draw = false;

		// box
		pjs.fill(0, 102, 204);
		pjs.rect(this.x, this.y, this.w, this.h);

		pjs.fill(50, 150, 250);
		pjs.rect(this.x, this.y, this.w, textHeight(this.name) + 6);

		// text
		pjs.fill(255, 255, 255);
		pjs.text(this.name, this.x + (this.w - textWidth(this.name)) / 2, this.y + textHeight(this.name) + 2);

		// is widget is selected ?
		if ( ccf_widget_selected == this.name ) {
			pjs.stroke(230, 230, 230);
			do_draw = true;

		// or is widget is currently selected for connectivity ?
		} else if ( connectivity_widget == this ) {
			pjs.stroke(0x00, 0xca, 0x6b);
			do_draw = true;

		// or is widget is currently in hover state ?
		} else {
			for ( var i = 0; i < widgets_hover.size(); i++ ) {
				o = widgets_hover[i];
				if ( this != o[0] )
					continue;
				pjs.stroke(0xd0, 0xd0, 0xd0);
				do_draw = true;
				break;
			}
		}

		if ( do_draw ) {
			pjs.noFill();
			pjs.rect(this.x-1, this.y-1, this.w+2, this.h+2);
			pjs.noStroke();
		}
	},

	headerSize: function() {
		return this.y + textHeight(this.name) + 10;
	},

	getInputX: function(idx) {
		return this.x + 15 / 2.;
	},

	getOutputX: function(idx) {
		return this.x + this.w - 15 / 2.;
	},

	getInputY: function(idx) {
		return this.headerSize() + idx * 20 + 15 / 2.;
	},

	getOutputY: function(idx) {
		return this.headerSize() + idx * 20 + 15 / 2.;
	},

	clearConnectivity: function() {
		for ( var i = 0; i < this.inputs.size(); i++ )
			this.inputs[i].clearConnectivity();
		for ( var i = 0; i < this.outputs.size(); i++ )
			this.outputs[i].clearConnectivity();
	}
});

function inList(pattern, list, separator) {
	list = list.split(separator);
	for ( var i = 0; i < list.length; i++ )
		if ( list[i] == pattern )
			return true;
	return false;
}

//
// link with UI
//

function widgetCreate(_name, have_gui) {
	var w = new Widget();
	w.name = _name;
	w.gui = have_gui;
	widgets.add(w);
}

function widgetGet(_name) {
	for ( var i = 0; i < widgets.size(); i++ )
		if ( widgets.get(i).name == _name )
			return widgets.get(i);
	return null;
}

function widgetPosition(_name, _x, _y) {
	var w = widgetGet(_name);
	if ( w == null )
		return;
	w.x = parseInt(_x);
	w.y = parseInt(_y);
}

function widgetAddInput(_key, _name, _type) {
	var w = widgetGet(_key);
	if ( w == null )
		return;
	var io = new IOWidget(_name, _type);
	w.inputs.add(io);
}

function widgetAddOutput(_key, _name, _type) {
	var w = widgetGet(_key);
	if ( w == null )
		return;
	var io = new IOWidget(_name, _type);
	w.outputs.add(io);
}

function widgetClearConnectivity() {
	for ( var i = 0; i < widgets.size(); i++ ) {
		var w = widgets.get(i);
		w.clearConnectivity();
	}
}

function widgetConnect(_out, _outidx, _in, _inidx) {
	var wout = widgetGet(_out),
		   win = widgetGet(_in);
	if ( win == null )
		return;
	wout.outputs.get(parseInt(_outidx)).connect(win, parseInt(_inidx));
}

function widgetCanvasResize(w, h) {
	try {
		pjs.size(w, h);
	} catch (e) { }
}

function widgetAutoLayout() {
	var margin = 50,
		padding = 50,
		widget_width = 120,
		widget_height = 75,
		x = margin,
		y = margin,
		ry,
		j = 0;

	for ( var i = 0; i < widgets.size(); i++ ) {
		var widget = widgets.get(i);
		ry = y + (j % 2) * 20;
		if ( widget.x != x || widget.y != ry ) {
			widget.x = x;
			widget.y = ry;
			ccf_set(widget.name, "x", widget.x, "y", widget.y);
		}

		x += widget_width + padding;
		if ( x > pjs.width - margin - widget_width ) {
			y += widget_height + padding;
			x = margin;
			if ( (j % 2) == 0 )
				j++;
		}
		j++;
	}

}

function widgetConfigure(guicommand, is_update) {
	if ( is_update == 0 && config_show == 1 ) {
		config_show = 0;
		return;
	}

	config_show = 1;
	config_insts = guicommand.split("\n");
	gui_draw();
}

function configDraw() {
	if ( config_show == 0 )
		return;

	//ccf_gui(ccf_widget_selected);

	// biggest task. decompile draw instruction
	// and convert to processing one.
	pjs.fill(255, 255, 255);
	var y = 10;
	var viewport_w = 1;
	var viewport_h = 1;
	function tb(x, a) {
		if (x <= 0) return 1;
		if (x >= a) return a - 1;
		return x
	}
	function ttw(s) { return tw(parseInt(s)); }
	function tth(s) { return th(parseInt(s)); }
	function tw(x) { return tb(x * cfg_screen_w / viewport_w, cfg_screen_w); }
	function th(x) { return tb(x * cfg_screen_h / viewport_h, cfg_screen_h); }
	for ( var index in config_insts ) {
		var args = config_insts[index].split(" ");

		switch ( args[0] ) {
			case '':
				break;

			case 'viewport':
				viewport_w = parseInt(args[1]);
				viewport_h = parseInt(args[2]);
				break;
				
			case 'color':
				pjs.fill(parseInt(args[1]), parseInt(args[2]), parseInt(args[3]));
				break;

			case 'rect':
				pjs.rect(ttw(args[1]), tth(args[2]), ttw(args[3]), tth(args[4]));
				break;

			case 'circle':
				pjs.ellipse(ttw(args[1]), tth(args[2]), ttw(args[3]), ttw(args[3]));
				break;

			case 'line':
				pjs.line(ttw(args[1]), tth(args[2]), ttw(args[3]), tth(args[4]));
				break;

			default:
				pjs.fill(150, 150, 150);
				pjs.text('cmd:' + args[0], 10, y);
				pjs.fill(255, 255, 255);
				y += 10;
		}
	}
}

function configSendFeedback(type, x, y) {
	ccf_set(ccf_widget_selected, "gui_feedback", "" + type + ";" + x + ";" + y, function() {
		// nothing.
	});
}


function textWidth(s) {
	return s.length * (cfg_fontsize / 2) + (s.length - 1);
}

function textHeight(s) {
	return cfg_fontsize;
}

function guiUpdate() {
	setTimeout('guiUpdate()', 500);
	if ( config_show )
		ccf_gui(ccf_widget_selected);
}

function gui_draw() {
	var w, x, y;

	pjs.background(51);

	if ( config_show == 1 ) {

		configDraw();

	} else {

		searchConnectivity();

		for ( var i = 0; i < widgets.size(); i ++ ) {
			w = widgets.get(i);
			if ( selected_widget == i ) {
				x = pjs.mouseX - selected_widget_x;
				y = pjs.mouseY - selected_widget_y;
				if ( w.x != x || w.y != y ) {
					w.x = x;
					w.y = y;
					w.positionChanged = true;
				}
			}
			w.draw();
		}

		checkConnectivity();
		drawConnectivity();

	}

	pjs.frameCount += 1;
	$('#framecount').html(pjs.frameCount);

	// reset mouse
	oldmouseX = pjs.mouseX;
	oldmouseY = pjs.mouseY;
}

function coolLine(x0, y0, x1, y1) {
	var left, delta, xmid, ymid;

	delta = 15;
	ydelta = (y1 - y0) / 10;

	xmid = x0 + (x1 - x0) / 2;

	if ( x1 - delta < x0 + delta ) {
		ymid = y0 + (y1 - y0) / 2;
		pjs.line(x0, y0, x0 + delta, y0);
		pjs.line(x0 + delta, y0, x0 + delta, ymid);
		pjs.line(x0 + delta, ymid, x1 - delta, ymid);
		pjs.line(x1 - delta, ymid, x1 - delta, y1);
		pjs.line(x0 + delta, ymid, x1 - delta, ymid);
		pjs.line(x1 - delta, y1, x1, y1);
	} else {
		xmid = x0 + (x1 - x0) / 2;
		pjs.line(x0, y0, xmid, y0);
		pjs.line(xmid, y0, xmid, y1);
		pjs.line(xmid, y1, x1, y1);
	}
	
	/**
	beginShape();
	if ( x1 - delta < x0 + delta ) {
		// FIXME
		ymid = y0 + (y1 - y0) / 2;
		curveVertex(x0, y0);
		curveVertex(x0, y0);
		curveVertex(x0 + delta, y0);
		curveVertex(x0 + delta, ymid);
		curveVertex(x1 - delta, ymid);
		curveVertex(x1 - delta, y1);
		curveVertex(x1, y1);
		curveVertex(x1, y1);
	} else {
		xmid = x0 + (x1 - x0) / 2;
		curveVertex(x0, y0);
		curveVertex(x0, y0);
		curveVertex(xmid - 10, y0 + ydelta);
		curveVertex(xmid + 10, y1 - ydelta);
		curveVertex(x1, y1);
		curveVertex(x1, y1);
	}
	endShape();
	**/
}

function drawConnectivity() {
	pjs.noFill();
	if ( selected_input_idx != -1 ) {
		coolLine(
			selected_input_widget.getInputX(selected_input_idx),
			selected_input_widget.getInputY(selected_input_idx),
			pjs.mouseX,
			pjs.mouseY
		);
	} else if ( selected_output_idx != -1 ) {
		coolLine(
			selected_output_widget.getOutputX(selected_output_idx),
			selected_output_widget.getOutputY(selected_output_idx),
			pjs.mouseX,
			pjs.mouseY
		);
	}

	var w, w2, io, idx;
	for ( var i = 0;  i < widgets.size(); i++ ) {
		w = widgets.get(i);
		for ( var j = 0; j < w.outputs.size(); j++ ) {
			io = w.outputs.get(j);
			for ( var k = 0; k < io.observers.size(); k++ ) {
				w2 = io.observers.get(k);
				idx = io.observers_idx.get(k);
				if ( connectivity_widget == w2 || connectivity_widget == w )
					pjs.stroke(color_selected);
				else
					pjs.stroke(color_idle);
				coolLine(w.getOutputX(j), w.getOutputY(j),
						 w2.getInputX(idx), w2.getInputY(idx));
			}
		}
	}

	pjs.noStroke();
}

function resetHighlight() {
	$.each(ccf_available_modules, function(name, module) {
		key = '#module_' + name;
		$(key).removeClass('highlight');
	});
}

function searchConnectivity() {
	var w;

	if ( (pjs.mouseX - oldmouseX) == 0 && (pjs.mouseY - oldmouseY) == 0 )
		return;

	connectivity_widget = null;
	connectivity_idx = -1;
	connectivity_io = '';

	for (var i = 0;  i < widgets.size(); i++ ) {
		w = widgets.get(i);
		if ( !w.collide(pjs.mouseX, pjs.mouseY) )
			continue;

		connectivity_widget = w;

		// selected an input ?
		idx = w.collideInput(pjs.mouseX, pjs.mouseY);
		if ( idx != -1 ) {
			connectivity_idx = idx;
			connectivity_io = 'input';
			break;
		}

		// selected an output ?
		idx = w.collideOutput(pjs.mouseX, pjs.mouseY);
		if ( idx != -1 ) {
			connectivity_idx = idx;
			connectivity_io = 'output';
			break;
		}
	}

	if ( connectivity_idx == -1 ) {
		resetHighlight();
		return;
	}

	// highlight module for this current index/module
	wselected = new Object();

	if ( connectivity_io == 'output' ) {
		var iow = connectivity_widget.getOutput(connectivity_idx);
		$.each(ccf_available_modules, function(name, module) {
			wselected['#module_' + name] = false;
			$(module.inputs).each(function(index, item) {
				if ( !inList(iow.type, item['type'], ',') )
					return;
				wselected['#module_' + name] = true;
			});
		});
	} else if ( connectivity_io == 'input' ) {
		var iow = connectivity_widget.getInput(connectivity_idx);
		$.each(ccf_available_modules, function(name, module) {
			wselected['#module_' + name] = false;
			$(module.outputs).each(function(index, item) {
				if ( !inList(item['type'], iow.type, ',') )
					return;
				wselected['#module_' + name] = true;
			});
		});
	}

	$.each(wselected, function(key, value) {
		if ( value )
			$(key).addClass('highlight');
		else
			$(key).removeClass('highlight');
	});
}

function checkConnectivity() {

	connectivity_ok = 0;

	pjs.strokeWeight(2);

	if ( connectivity_idx == -1 ) {
		pjs.stroke(color_idle);
		return;
	}

	if ( selected_input_idx != -1 ) {
		if ( connectivity_io != 'output' ) {
			pjs.stroke(color_ko);
			return;
		}
		if ( selected_input_widget == connectivity_widget ) {
			pjs.stroke(color_ko);
			return;
		}
	} else if ( selected_output_idx != -1 ) {
		if ( connectivity_io != 'input' ) {
			pjs.stroke(color_ko);
			return;
		}
		if ( selected_output_widget == connectivity_widget ) {
			pjs.stroke(color_ko);
			return;
		}
	}

	connectivity_ok = 1;
	pjs.stroke(color_ok);
}

function searchWidgetConnection(w, iow, iotype) {
	// update hover list to highlight all the widgets / io that
	// can be used to link the current Widget / IO
	for ( var i = 0; i < widgets.size(); i++ ) {
		w2 = widgets.get(i);
		if ( w == w2 )
			continue;
		count = 0;
		if ( iotype == 'output' )
			count = w2.getOutputCount();
		else
			count = w2.getInputCount();
		for ( var j = 0; j < count; j++ ) {
			if ( iotype == 'output' )
				iow2 = w2.getOutput(j);
			else
				iow2 = w2.getInput(j);
			if ( iotype == 'output' ) {
				if ( !inList(iow2.type, iow.type, ',') )
					continue
			} else {
				if ( !inList(iow.type, iow2.type, ',') )
					continue
			}
			widgets_hover.add([w2, iotype, j]);
		}
	}
}

function selectWidget(index) {
	if ( index == -1 ) {
		$('#module').html('');
		ccf_select('');
		return;
	}

	var w = widgets.get(index);
	var name = w.name;
	ccf_select(name);

	var data = [
		'<h1>Details of ', name, '</h1>',
		'<div style="text-align: center; padding-top: 10px;">',
		'<button id="deletemodule">Delete</button>',
		'</div>'
	];

	function pushIO(data, index, io) {
		data.push('<li>');
		data.push('<span class="ioindex">');
		data.push(index);
		data.push('</span>: <span class="ioname">');
		data.push(io.name);
		data.push('</span> <span class="iotype">(');
		data.push(io.type);
		data.push(')</span>');
		data.push('</li>');
	}

	if ( w.getInputCount() ) {
		data.push('<h2>Inputs</h2><ul>');
		for ( var i = 0; i < w.getInputCount(); i++ ) {
			var io = w.getInput(i);
			pushIO(data, i, io);
		}
		data.push('</ul>');
	}

	if ( w.getOutputCount() ) {
		data.push('<h2>Outputs</h2><ul>');
		for ( var i = 0; i < w.getOutputCount(); i++ ) {
			var io = w.getOutput(i);
			pushIO(data, i, io);
		}
		data.push('</ul>');
	}

	$('#module').html(data.join(''));

	$('#deletemodule').button().mousedown(function() {
		ccf_remove(name);
		widgets.remove(widgets.indexOf(w));
		selected_widget = null;
		$('#module').html('');
	});
}



function gui_mousePressed() {
	var w, w2, iow, iow2, idx;

	if ( config_show == 1 ) {
		configSendFeedback('down', pjs.mouseX, pjs.mouseY);
		return;
	}

	widgets_hover.clear();

	for ( var i = widgets.size() - 1; i >= 0; i-- ) {
		w = widgets.get(i);
		if ( w.collide(pjs.mouseX, pjs.mouseY) ) {
			// selected an input ?
			idx = w.collideInput(pjs.mouseX, pjs.mouseY);
			if ( idx != -1 ) {
				selected_input_widget = w;
				selected_input_idx = idx;
				iow = w.getInput(idx);
				searchWidgetConnection(w, iow, 'output');
				return;
			}

			// selected an output ?
			idx = w.collideOutput(pjs.mouseX, pjs.mouseY);
			if ( idx != -1 ) {
				selected_output_widget = w;
				selected_output_idx = idx;
				iow = w.getOutput(idx);
				searchWidgetConnection(w, iow, 'input');
				return;
			}

			// so, the widget is selected !
			selected_widget = i;
			selected_widget_x = pjs.mouseX - w.x;
			selected_widget_y = pjs.mouseY - w.y;
			selectWidget(selected_widget);
			return;
		}
	}

	selected_input_idx = -1;
	selected_output_idx = -1;
	selected_widget = -1;
	selectWidget(-1);
}

function gui_mouseReleased() {

	widgets_hover.clear();

	if ( config_show == 1 ) {
		configSendFeedback('up', pjs.mouseX, pjs.mouseY);
		gui_draw();
		return;
	}

	if ( connectivity_ok == 1 ) {
		if ( connectivity_widget != selected_output_widget ) {
			if ( selected_output_idx != -1 ) {
				selected_output_widget.outputs.get(selected_output_idx).connect(
					connectivity_widget, connectivity_idx);
				ccf_connect(connectivity_widget.name, connectivity_idx, selected_output_widget.name, selected_output_idx);
			} else if ( selected_input_idx != -1 ) {
				connectivity_widget.outputs.get(connectivity_idx).connect(
					selected_input_widget, selected_input_idx);
				ccf_connect(selected_input_widget.name, selected_input_idx, connectivity_widget.name, connectivity_idx);
			}
		}
		connectivity_ok = 0;
	}

	if ( selected_widget != -1 ) {
		var w = widgets.get(selected_widget);
		if ( w.positionChanged == true ) {
			ccf_set(w.name, "x", w.x, "y", w.y);
			w.positionChanged = false;
		}
	}

	selected_widget = -1;
	selected_input_idx = -1;
	selected_output_idx = -1;

	gui_draw();
}

function gui_mouseDragged() {
	gui_draw();
	if ( config_show == 1 ) {
		configSendFeedback('move', pjs.mouseX, pjs.mouseY);
		return;
	}
}


// setup processing canvas
function ccf_pjs() {
	color_ok = pjs.color(0, 0xcc, 0);
	color_ko = pjs.color(0x00, 0, 0xCC);
	color_idle = pjs.color(0x00, 0x80, 0xC5);
	color_selected = pjs.color(0x00, 0xea, 0x8b);
	widgets = new pjs.ArrayList();
	widgets_hover = new pjs.ArrayList();

	pjs.noStroke();
	pjs.textFont(pjs.loadFont("monospace"), cfg_fontsize);
	//setTimeout('update()', 500);
}

function ccf_pjs_reset() {
	widgets = new pjs.ArrayList();
	widgets_hover = new pjs.ArrayList();
}

