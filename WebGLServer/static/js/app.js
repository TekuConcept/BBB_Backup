requirejs.config({
    paths: {
        "jquery": "https://ajax.googleapis.com/ajax/libs/jquery/1.7.2/jquery.min"
    }
});

require([
    'jquery',
    'modules/game',
    'modules/module'
],
function($, game, mod) {
    var userCam  = getCamera();
	var controls = getControls(userCam, game);
	
    game.init({
        camera: userCam,
        init: [
            mod.init
        ],
        updateCallbacks: [
            controls.update,
            mod.update
        ],
        resizeCallbacks: [
            controls.handleResize
        ]
    });
    game.animate();
});

function getCamera() {
    var camera = new THREE.PerspectiveCamera( 75,
        window.innerWidth / window.innerHeight, 1, 10000 );
	camera.position.z = 20;
	return camera;
}
function getControls(cam, game) {
    var controls = new THREE.TrackballControls( cam );
	controls.rotateSpeed = 2.0;
	controls.zoomSpeed = 1.2;
	controls.panSpeed = 1;
	controls.noZoom = false;
	controls.noPan = true;
	controls.staticMoving = true;
	controls.dynamicDampingFactor = 0.3;
	controls.keys = [ 65, 83, 68 ];
	controls.addEventListener( 'change', game.render );
	return controls;
}