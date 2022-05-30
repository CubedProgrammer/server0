canv = document.getElementById('canv')
ctx = canv.getContext('2d')
canvasBG = 
{
    red: 255,
    green: 0,
    blue: 0
}
rainbowFunc = function()
{
    var num = canvasBG.red * 65536 + canvasBG.green * 256 + canvasBG.blue + 16777216
    var colstr = num.toString(16).substring(1)
    ctx.fillStyle = '#' + colstr
    ctx.fillRect(0, 0, canv.width, canv.height)
    if(canvasBG.red == 255)
    {
        if(canvasBG.green == 255)
            --canvasBG.red
        else if(canvasBG.blue > 0)
            --canvasBG.blue
        else
            ++canvasBG.green
    }
    else if(canvasBG.green == 255)
    {
        if(canvasBG.blue == 255)
            --canvasBG.green
        else if(canvasBG.red > 0)
            --canvasBG.red
        else
            ++canvasBG.blue
    }
    else if(canvasBG.blue == 255)
    {
        if(canvasBG.green > 0)
            --canvasBG.green
        else
            ++canvasBG.red
    }
}
rainbow = setInterval(rainbowFunc, 17)
