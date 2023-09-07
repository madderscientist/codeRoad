// 专管ui
window.addEventListener('load', () => {
    // 拖动控制台大小
    let consoleDiv = document.getElementById('console');
    let sparateBar = document.getElementById('separateBar');
    let zhanwei = document.getElementById('zhanwei');
    zhanwei.style.height = consoleDiv.style.height = `${parseInt(document.body.clientHeight/2)}px`;
    let resize = (e) => {
        let y = e.clientY;
        if(y==undefined) y = e.touches[0].clientY;
        if (y) {
            let newHeight = document.body.clientHeight - y + 'px';
            consoleDiv.style.height = newHeight;
            zhanwei.style.height = newHeight;
        }
    }
    sparateBar.addEventListener('drag', resize);
    sparateBar.addEventListener('touchmove', resize);
    // 发送事件
    let senfbtn = document.getElementById('sendCommants');
    let typein = document.getElementById('commands');
    let shell = document.getElementById('shell');
    senfbtn.addEventListener('click', ()=>{
        shell.innerHTML += '<br>' + typein.value;
        typein.value = '';
    });
    document.addEventListener('keydown', (e)=>{
        if(e.key=='Enter') senfbtn.click();
    })
});

function changeBar(id, bili){
    function getColorByBaiFenBi(bili){
        var one = (255+255) / 100;  
        var r=0;
        var g=0;
        var b=0;
        if ( bili < 50 ) { 
            r = one * bili;
            g=255;
        }
        if ( bili >= 50 ) {
            g =  255 - ( (bili - 50 ) * one) ;
            r = 255;
        }
        r = parseInt(r);
        g = parseInt(g);
        b = parseInt(b);
        return "rgb("+r+","+g+","+b+")";    
    }
    var div = document.getElementById(id);
    div.style.background = `linear-gradient(90deg, ${getColorByBaiFenBi(Math.max(bili-25,0))}, ${getColorByBaiFenBi(bili)})`
    div.style.width = bili + '%';
}
function addPlayers(names) {
    let list = document.getElementById('playerlist');
    for(p of names){
        let div = document.createElement('div');
        div.innerHTML = p;
        div.className = "onePlayer";
        list.appendChild(div);
    }
}