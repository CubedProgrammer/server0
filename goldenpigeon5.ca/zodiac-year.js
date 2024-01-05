animals = ['Rat', 'Ox', 'Tiger', 'Rabbit', 'Dragon', 'Snake', 'Horse', 'Goat', 'Monkey', 'Rooster', 'Dog', 'Pig']
YBEGIN = 4
TRBEGIN = '<tr>', TREND = '</tr>'
TDBEGIN = '<td>', TDEND = '</td>'
function gzodiac()
{
    var box = document.getElementById('year')
    var ens = '', num = parseInt(box.value)
    for(var i = 0; i < 12; i++)
    {
        var y = num + i, ind = (y - YBEGIN) % 12 + 12
        ind %= 12
        var ytd = TDBEGIN + y + TDEND, atd = TDBEGIN + animals[ind] + TDEND
        ens += TRBEGIN + ytd + atd + TREND
    }
    document.getElementById('zodiacs').innerHTML = ens;
}
gzodiac()
