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

// Cookie expiration in 60 days
// days = 60;
// CookieExpires = new Date();
// CookieExpires.setTime( CookieExpires.getTime() + ( 60 * 60 * 1000 * 24 * days ) );
// $.cookies.setOptions({ expiresAt: CookieExpires });

// guess ourself the base url, needed for xhr
var ccf_baseurl = location.href.split('/', 3).join('/');
var ccf_available_modules = {};
var ccf_available_inputs = [];
var ccf_available_outputs = [];
var ccf_streamscale = 2;
var ccf_widget_selected = null;
var ccf_status_text = 'stopped';
var ccf_uniqidx = 0;
var ccf_data = null;
var ccf_host = null;
var ccf_port = '7500';
var ccf_slot_dialog = null;
var ccf_slot_action = 'load';
var pjs = null;

function ccfModule(name) {
	this.name = name;
	this.inputs = [];
	this.outputs = [];
}

function ccf_uniq() {
	ccf_uniqidx += 1;
	return 'ccf' + ccf_uniqidx;
}

function ccf_resize() {
	$('#ccfcanvas').attr('width', 10);
	$('#ccfcanvas').attr('height', 10);

	// hardcoded...
	var h = $(window).height() - 100;
	var w = $(window).width() - 82;
	w -= $('#leftcolumn').width();
	w -= $('#rightcolumn').width();
	$('#ccfcanvas').attr('width', w);
	$('#ccfcanvas').attr('height', h);
	if ( widgetCanvasResize != null )
		widgetCanvasResize(w, h);
}

function ccf_bootstrap() {

	var hostport = ccf_baseurl.split('/');
	hostport = hostport[hostport.length - 1].split(':');
	if ( hostport.length == 1 ) {
		ccf_host = hostport[0];
	} else {
		ccf_host = hostport[0];
		ccf_port = hostport[1];
	}

	// prepare selection
	$('#container-preview').hide();
	$('#btn-module').addClass('ui-state-active');
	$('#btn-properties').addClass('ui-state-active');
	$('#btn-create').addClass('ui-state-active');
	$('#pipeline-content-text').hide();
	$('#menu-pipeline').menu({
		content: $('#menu-pipeline-content').html(),
		flyOut: true,
		showSpeed: 0,
		crossSpeed: 0,
	})

	$('#menu-windows').menu({
		content: $('#menu-windows-content').html(),
		flyOut: true,
		showSpeed: 0,
		crossSpeed: 0,
	})

	$(window).resize(ccf_resize);

	// prepare buttons (hover + actions)
	$('.ccf-action')
	.hover(
		function(){
			$(this).addClass('ui-state-hover');
		},
		function(){
			$(this).removeClass('ui-state-hover');
		}
	)
	.mousedown(function(){
		switch ($(this).attr('id')) {
			case 'btn-start':
				ccf_start();
				break;
			case 'btn-stop':
				ccf_stop();
				break;
			case 'btn-autolayout':
				ccf_layout();
				break;
			case 'slot-1':
			case 'slot-2':
			case 'slot-3':
			case 'slot-4':
			case 'slot-5':
			case 'slot-6':
			case 'slot-7':
			case 'slot-8':
			case 'slot-9':
			case 'slot-10':
				var slotidx = $(this).attr('id').split('-')[1];
				ccf_slot_dialog.dialog('close');
				if ( ccf_slot_action == 'load' )
					ccf_slot_load(slotidx);
				else
					ccf_slot_save(slotidx);
				break;

			default:
				break;
		}
	});

	pjs = Processing($('#ccfcanvas')[0], $('#ccfpjs')[0].text);
	ccf_pjs();

	setTimeout(ccf_resize, 50);
	ccf_modules();
	ccf_status();
	ccf_stats();
}

function ccf_menu_select(item, itemid) {
	switch (itemid) {
		case 'pipeline-text':
			$(this).addClass('ui-state-active');
			$('#pipeline-content-textarea').width($('#pipeline-content-canvas').width());
			$('#pipeline-content-textarea').height($('#pipeline-content-canvas').height());
			$.get(ccf_baseurl + '/pipeline/dump', function(data) {
				$('#pipeline-content-textarea').html(data);
				$('#pipeline-content-text').dialog({
					title: 'Pipeline text',
					modal: true,
					width: $('#pipeline-content-canvas').width() + 40,
					height: $('#pipeline-content-canvas').height() + 40,
				});
			});
			break;
		case 'pipeline-download':
			window.location = ccf_baseurl + '/pipeline/dump?download=1';
			break;
		case 'pipeline-slot-load':
			ccf_slot_refresh(function() {
				ccf_slot_action = 'load';
				ccf_slot_dialog = $('#pipeline-slot-content').dialog({
					title: 'Load pipeline from a slot',
					modal: true
				});
			});
			break;
		case 'pipeline-slot-save':
			ccf_slot_refresh(function() {
				ccf_slot_action = 'save';
				ccf_slot_dialog = $('#pipeline-slot-content').dialog({
					title: 'Save pipeline to slot',
					modal: true
				});
			});
			break;
		case 'btn-module':
		case 'btn-create':
		case 'btn-properties':
		case 'btn-preview':
			var container = '#container-' + itemid.split('-')[1];
			if ( $(item).hasClass('ui-state-active') ) {
				$(container).hide();
				$(item).removeClass('ui-state-active');
			} else {
				$(container).show();
				$(item).addClass('ui-state-active');
			}
			ccf_resize();
			break;

	}
}

function ccf_modules() {
	$.get(ccf_baseurl + '/factory/list', function(data) {
		$('#modules').html('');
		$(data['list']).each(function (index, elem) {
			$('#modules').append(
				$('<a></a>')
				.html(elem)
				.addClass('module')
				.attr('id', 'module_' + elem)
				.attr('href', 'javascript:ccf_create("' + elem + '")')
			);
		});

		$.each(data['details'], function (name, infos) {
			var module = new ccfModule(name);
			$(infos['inputs']).each(function (index, elem) {
				module.inputs.push(elem);
			});
			$(infos['outputs']).each(function (index, elem) {
				module.outputs.push(elem);
			});
			ccf_available_modules[name] = module;
		});
		ccf_resize();
	});
}

function ccf_menu_set_running(running) {
	if ( running ) {
		$('#btn-stop').removeClass('ui-state-active');
		$('#btn-start').addClass('ui-state-active');
	} else {
		$('#btn-stop').addClass('ui-state-active');
		$('#btn-start').removeClass('ui-state-active');
	}
}

function ccf_status() {
	$.get(ccf_baseurl + '/pipeline/status', function(data) {
		ccf_available_inputs = [];
		ccf_available_outputs = [];

		ccf_status_text = data['status']['running'] == '0' ? 'stopped' : 'running'
		ccf_menu_set_running(data['status']['running'] != '0');
		$('#version').html(data['status']['version']);

		widgetClearConnectivity();

		for ( key in data['status']['modules'] ) {
			var infos = data['status']['modules'][key];
			if ( widgetGet(key) == null ) {
				var _x = infos['properties']['x'];
				var _y = infos['properties']['y'];
				var _gui = infos['gui'];
				if ( typeof _x == 'undefined' )
					_x = 0;
				if ( typeof _y == 'undefined' )
					_y = 0;
				widgetCreate(key, _gui);
				widgetPosition(key, _x, _y);

				if ( typeof(infos['inputs']) != 'undefined' ) {
					for ( idx in infos['inputs'] ) {
						input = infos['inputs'][idx];
						widgetAddInput(key, input['name'], input['type']);
					}
				}
				if ( typeof(infos['outputs']) != 'undefined' ) {
					for ( idx in infos['outputs'] ) {
						input = infos['outputs'][idx];
						widgetAddOutput(key, input['name'], input['type']);
					}
				}
			}
		}

		for ( key in data['status']['modules'] ) {
			var infos = data['status']['modules'][key];
			if ( typeof(infos['outputs']) != 'undefined' ) {
				for ( idx in infos['outputs'] ) {
					var output = infos['outputs'][idx];
					for ( k in output['observers'] ) {
						widgetConnect(key, idx, output['observers'][k], 0);
					}
				}
			}
		}
	});
}

function ccf_create(elem) {
	$.get(ccf_baseurl + '/pipeline/create?objectname=' + elem, function(data) {
		ccf_status();
		ccf_select(data['message']);
	});
}

function ccf_remove(elem) {
	$.get(ccf_baseurl + '/pipeline/remove?objectname=' + elem, function(data) {
		ccf_select('');
	});
}

function ccf_properties(elem) {
	if ( elem == '' ) {
		$('#properties').html('');
		$('#properties').slideUp('fast');
		ccf_status();
		return;
	}

	// ask for the status of the pipeline,
	// filter on the UI we want, and contruct properties list.
	$.get(ccf_baseurl + '/pipeline/status', function(data) {
		for ( key in data['status']['modules'] ) {
			if ( key != elem )
				continue;

			// extract info about our module
			infos = data['status']['modules'][key];

			// all elements will be in a table, prepare it
			var table = $('<table></table>');
			if ( infos['gui'] == 1 ) {
				var uniq = ccf_uniq();
				var input =
					$('<input></input>')
					.attr('id', uniq)
					.attr('type', 'checkbox')
					.attr('onchange', 'javascript:ccf_gui("'
						+ elem + '", 0)');
				var tr = $('<tr></tr>')
					.append( $('<td></td>')
						.addClass('label')
						.html('GUI')
					)
					.append( $('<td></td>')
						.append(input)
						.append(
							$('<label></label>')
							.attr('for', uniq)
							.html('Open'))
					);
				table.append(tr);
			}


			// enumerate properties
			for ( var property in infos['properties'] ) {
				var tr = $('<tr></tr>');
				var td = $('<td></td>');

				// add the label into the table
				value = infos['properties'][property];
				tr.append($('<td></td>')
					.addClass('label')
					.html(property)
				);

				// extract properties infos
				pinfo = infos['propertiesInfos'][property];

				//
				// bool
				//
				if ( pinfo['type'] == 'bool' ) {
					var uniq = ccf_uniq();
					var input =
						$('<input></input>')
						.attr('id', uniq)
						.attr('type', 'checkbox')
						.attr('checked', value=='true'?'checked':'')
						.attr('onchange', 'javascript:ccf_set("'
							+ elem + '", "' + property
							+ '", this.checked ? "true" : "false")');
					td.append(input);
					td.append(
						$('<label></label>')
						.attr('for', uniq)
						.html('Activate')
					);


				//
				// double
				//
				} else if ( pinfo['type'] == 'double' ) {
					var slider = $('<div></div>').slider().slider('option', 'value', value);
					if ( typeof pinfo['min'] != 'undefined' )
						slider.slider('option', 'min', pinfo['min']);
					if ( typeof pinfo['max'] != 'undefined' )
						slider.slider('option', 'max', pinfo['max']);
					var _p = property;
					var _e = elem;
					slider.bind('slidechange', function(event, ui) {
							ccf_set(_e, _p, ui.value);
					});
					td.append(slider);

				//
				// choice list, use a select
				//
				} else if ( typeof pinfo['choices'] != 'undefined' ) {
					var s = $('<select></select>')
						.addClass('ui-widget ui-widget-content')
						.attr('onchange', 'javascript:ccf_set("'
							+ elem + '", "' + property
							+ '", this.value)')
					var choices = pinfo['choices'].split(';');
					for ( var i = 0; i < choices.length; i++ ) {
						choice = choices[i];
						s.append($('<option></option>')
							.attr('value', choice)
							.attr('selected', value == choice?'selected':'')
							.html(choice)
						);
					}
					td.append(s);

				//
				// default case, use a simple input
				//
				} else {
					td.append(
						$('<input></input>')
						.addClass('text ui-widget-content')
						.attr('type', 'text')
						.attr('value', value)
						.attr('onblur', 'javascript:ccf_set("'
							+ elem + '", "' + property
							+ '", this.value)')
					);
				}

				// add the property to the table
				tr.append(td);
				table.append(tr);
			}

			// show table !
			$('#properties').html(table);
		}
	});

	// slide slide :)
	$('#properties').slideDown('fast');

	// WHYYYYYYYYYYYY ? :' :' :(
	setTimeout(_ccf_update_state, 20);
}

function _ccf_update_state() {
	$('#properties input[type=\"checkbox\"]').button();
}

function ccf_set(id) {
	var callback = null;
	var url = ccf_baseurl + '/pipeline/set?objectname=' + id;

	if ( typeof(arguments[arguments.length - 1]) == "function" )
		callback = arguments[arguments.length - 1];

	for ( var i = 1; i < arguments.length; i += 2 ) {
		url += '&name=' + arguments[i];
		url += '&value=' + arguments[i+1];
	}

	$.get(url, function(data) {
		if ( callback != null )
			callback();
	});
}

function ccf_connect(input, inidx, output, outidx) {

	$.get(ccf_baseurl + '/pipeline/connect?in=' + input + '&out=' + output + '&inidx=' + inidx + '&outidx=' + outidx, function(data) {
	});
}

function ccf_start() {
	$.get(ccf_baseurl + '/pipeline/start', function(data) {
		ccf_status();
	});
}

function ccf_stop() {
	ccf_stream('');
	$.get(ccf_baseurl + '/pipeline/stop', function(data) {
		ccf_status();
	});
}

function ccf_stream(elem) {
	if ( ccf_status_text == 'stopped' ) {
		$('#video').slideUp('fast');
		return;
	}
	if ( elem == '' ) {
		$('#streamid').html('No video');
		$('#streamimg').attr('src', '/gui/nostream.png');
		$('#video').slideUp('fast');
	} else {
		$('#streamid').html('Video of ' + elem);
		$('#streamimg').attr('src', ccf_baseurl + '/pipeline/stream?objectname=' + elem + '&scale=' + ccf_streamscale + '#' + Math.random() * 10000);
		$('#video').slideDown('fast');
	}
}

function ccf_select(elem) {
	ccf_widget_selected = elem;
	ccf_properties(elem);
	ccf_stream(elem);
	ccf_gui_cancel();
}

function ccf_gui_cancel() {
	var base = document.getElementById('flashgui');
	if ( base )
		base.parentNode.removeChild(base);
}

function ccf_gui(elem, is_update) {
	/**
	$.get(ccf_baseurl + '/pipeline/gui?objectname=' + elem, function(data) {
		widgetConfigure(data, is_update);
	});
	**/

	// toggle GUI
	var base = document.getElementById('flashgui');
	if ( base ) {
		base.parentNode.removeChild(base);
		return;
	}

	// construct basics parameters
	var src = ccf_baseurl + '/gui/flashgui.swf?objectname=' + elem;
	var flashvars = [
		'objectname=', encodeURIComponent(elem),
		'&ip=', ccf_host,
		'&port=', ccf_port
	].join('');

	// Internet explorer support
	base = document.createElement('OBJECT');
	base.setAttribute('id', 'flashgui');
	base.setAttribute('width', '640');
	base.setAttribute('height', '480');
	param = document.createElement('PARAM');
	param.setAttribute('name', 'movie');
	param.setAttribute('value', src);
	base.appendChild(param);
	param = document.createElement('PARAM');
	param.setAttribute('name', 'FlashVars');
	param.setAttribute('value', flashvars);
	base.appendChild(param);
	param = document.createElement('PARAM');
	param.setAttribute('name', 'allowFullScreen');
	param.setAttribute('value', 'true');
	base.appendChild(param);

	// Firefox support
	embed = document.createElement('EMBED');
	embed.setAttribute('src', src);
	embed.setAttribute('flashvars', flashvars);
	embed.setAttribute('allowfullscreen', 'true');
	embed.setAttribute('width', '640');
	embed.setAttribute('height', '480');
	base.appendChild(embed);

	document.body.appendChild(base);
	$('#flashgui').dialog({
		modal: true,
		width: 660,
		height: 480,
		title: elem + '\'s GUI',
	});
}

function ccf_stats() {
	$.get(ccf_baseurl + '/pipeline/stats', function(data) {
		var report = 'Utterances Processed: -';
		var count = 0,
			average_fps = 0,
			average_process_time = 0,
			average_wait_time = 0;
		for ( key in data.stats ) {
			count++;
			average_fps += parseFloat(data['stats'][key]['average_fps']);
			average_process_time += parseFloat(data['stats'][key]['average_process_time']);
			average_wait_time += parseFloat(data['stats'][key]['average_wait_time']);
		}
		// TODO, show process/wait time
		if ( ccf_status_text == 'running' && count > 0 )
			report = 'FPS: ' + (average_fps / count).toFixed(2);
		$('#toolbareport').html(report);
		setTimeout(ccf_stats, 2000);
	});
}

function ccf_refresh() {
	ccf_pjs_reset();
	ccf_properties('');
	ccf_status();
}

function ccf_slot_load(idx) {
	// do the real load
	$.get(ccf_baseurl + '/pipeline/slot/load?idx=' + idx, function(data) {
		ccf_refresh();
	});
}

function ccf_slot_save(idx) {
	$.get(ccf_baseurl + '/pipeline/slot/save?idx=' + idx, function(data) {
	});
}

function ccf_slot_refresh(callback) {
	$.get(ccf_baseurl + '/pipeline/slot/refresh', function(data) {
		// refresh data
		for ( slotidx in data['slots'] ) {
			var slot = data['slots'][slotidx];
			var slotinfo = 'Never used';
			var lastmod = parseInt(slot['lastmod']);
			if ( lastmod > 0 ) {
				var date = new Date(lastmod * 1000);
				slotinfo = date.toDateString() + ' ' +
						   date.getHours() + ':' +
						   date.getMinutes() + ':' +
						   date.getSeconds();
			}
			$('#slot-' + (parseInt(slotidx)+1) + ' .details').html(slotinfo);
		}
		if (callback)
			callback();
	});
}

function ccf_layout() {
	widgetAutoLayout();
}

// once our document is loaded, load our software.
$(document).ready(function() {
	ccf_bootstrap();
});
