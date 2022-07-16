resElem = document.getElementById('factors')
numElem = document.getElementById('num')
keyhand = (e)=>
{
    if(e.keyCode === 13)
        factor()
}
document.addEventListener('keyup', keyhand)
function factor()
{
    var num = parseInt(numElem.value)
    var primes = []
    var exps = []
    var curr = num, end = Math.sqrt(num)
    for(var i = 2; i <= end; ++i)
    {
        if(curr % i === 0)
        {
            primes.push(i)
            exps.push(0)
            for(; curr % i === 0; exps[exps.length - 1]++, curr /= i);
        }
    }
    if(curr > 1)
    {
        exps.push(1)
        primes.push(curr)
    }
    resElem.innerHTML = 'Factors:'
    for(var i = 0; i < primes.length; ++i)
    {
        resElem.innerHTML += ' ';
        resElem.innerHTML += primes[i];
        if(exps[i] !== 1)
            resElem.innerHTML += `<sup>${exps[i]}</sup>`
    }
}
