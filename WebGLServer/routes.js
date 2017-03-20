module.exports = function(app,msngr) {
    app.get('/', function(request, response) {
        response.render('home', {
            // page params
        });
    });
    
    // app.get('/compass', function(request, response) {
    //     response.send(msngr.compass());
    // });
    // app.post('/control_mode', function(request,response) {
    //     var flag = false;
    //     if(request.body)
    //         flag = request.body.control_mode === 1;
    //     else
    //         console.log(request.body);
    //     response.send("");
    //     msngr.setAutopilot(flag);
    // });
}