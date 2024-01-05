tot = document.getElementById('tottime')
inc = document.getElementById('inctime')
but = document.getElementById('start')
white = document.getElementById('white')
black = document.getElementById('black')
texts = [white, black]
times = []
turn = 0
clock = 0
tottime = parseInt(tot.value)
inctime = parseInt(inc.value)
function timetext(cnt)
{
    var min = Math.floor(cnt / 60), sec = cnt % 60
    var str = min.toString() + ':'
    if(sec < 10)
        str += '0'
    return str + sec.toString()
}
function tickclock()
{
    --times[turn]
    texts[turn].innerText = timetext(times[turn])
    if(times[turn] === 0)
    {
        clearInterval(clock)
        texts[turn].style.color = 'red'
        times.pop()
        times.pop()
        but.innerText = 'Start'
    }
}
function startclock()
{
    if(times.length === 0)
    {
        but.innerText = 'Move'
        times = [tottime, tottime]
        black.innerText = white.innerText = timetext(tottime)
        clock = setInterval(tickclock, 1000)
    }
    else
    {
        times[turn] += inctime
        texts[turn].innerText = timetext(times[turn])
        ++turn
        turn &= 1
    }
}
