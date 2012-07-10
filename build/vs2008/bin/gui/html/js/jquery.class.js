(function($){
	$.buildClass = function(){
		var argc = 0;
		
		var methods = arguments[argc]?arguments[argc++]:{};
		var Parent = $.isFunction(arguments[argc])?arguments[argc]:function(){};
		
		function Uber(base){
			if(Parent.prototype.__Uber) this.uber = new Parent.prototype.__Uber(base);
			this.object = base;
		}
		
		for(var f in Parent.prototype){
			if($.isFunction(Parent.prototype[f])){
				//Use eval to generate all functions « on the fly ». Otherwize you're screwed up.
				Uber.prototype[f] = eval('(function(){return function(){return Parent.prototype[\''
								+ f.replace(/\\'/g, '\\\'').replace(/\\/g, '\\')
								+ '\'].apply(this.object, arguments);};})();' );
			}
		}
		
		var Constructor = function(){
			this.uber = new this.__proto__.__Uber(this);
			if(methods.constructor) methods.constructor.apply(this, arguments);
		}
		
		var F = function(){}
		F.prototype = Parent.prototype;
		Constructor.prototype = new F();
		
		$.extend(Constructor.prototype, methods, { __Uber : Uber });
		Constructor.prototype.constructor = Constructor;
	
		return Constructor;
	}
})(jQuery);
