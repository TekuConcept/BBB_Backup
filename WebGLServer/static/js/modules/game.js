define([], function() {
    var FOV = 75,
        NEAR = 1,
        FAR = 10000;
    var scene,
        renderer,
        camera;
	var updateCallbacks,
	    resizeCallbacks;

    var init = function(config) {
    	baseInit();
    	
    	updateCallbacks = config.updateCallbacks;
    	resizeCallbacks = config.resizeCallbacks;
    	camera          = config.camera;
    	
    	if(config.init) {
        	var len = config.init.length;
    		for(i = 0; i < len; i++)
    		    config.init[i](scene);
    	}
    };

	var animate = function() {
		requestAnimationFrame( animate );
		update();
		render();
	};

    function baseInit() {
        scene           = new THREE.Scene();
    	renderer        = new THREE.CanvasRenderer();
    	camera          = new THREE.PerspectiveCamera( FOV, window.innerWidth / window.innerHeight, NEAR, FAR );
    	var container   = document.createElement('div');
    	document.body.appendChild(container);
    	renderer.setPixelRatio( window.devicePixelRatio );
    	renderer.setSize( window.innerWidth, window.innerHeight );
    	container.appendChild( renderer.domElement );
    
    	window.addEventListener( 'resize', onWindowResize, false );
    }

	function update() {
	    var len = updateCallbacks.length;
	    for(i = 0; i < len; i++)
	        updateCallbacks[i]();
	}
	
	function render() {
		renderer.render( scene, camera );
	}

	function onWindowResize() {
		renderer.setSize( window.innerWidth, window.innerHeight );
		
		var len = resizeCallbacks.length;
		for(i = 0; i < len; i++)
		    resizeCallbacks[i]();
	}
	
	return {
	    init: init,
	    animate: animate,
	    render: render
	};
});