const nbOfElts = 300;
firebase.database().ref('DHT11/measures').limitToLast(nbOfElts).on('value', ts_measures => {

    console.log(ts_measures.val());
    let timestamps = [];
    let valuestemp = [];
    let valueshum = [];


    ts_measures.forEach(ts_measure => {
        timestamps.push(moment(ts_measure.val().timestamp).format('YYYY-MM-DD HH:mm:ss'));
        valuestemp.push(ts_measure.val().temp);
        valueshum.push(ts_measure.val().hum);
    });
    myPlotTemp = document.getElementById('myPlotTemp');
    const dataTemp = [{
        x: timestamps,
        y: valuestemp
    }];

    const layout = {
        xaxis: {
            title: '<b>Čas</b>',
            linecolor: 'black',
            linewidth: 2
        },
        yaxis: {
            title: '<b>Teplota</b>',
            titlefont: {
                size: 14,
                color: '#000'
            },
            linecolor: 'black',
            linewidth: 2,
        },
        margin: {
            r: 50,
            pad: 0
        }
    }
    // At last we plot data :-)
    Plotly.newPlot(myPlotTemp, dataTemp, layout, { responsive: true });

    myPlotHum = document.getElementById('myPlotHum');
    const dataHum = [{
        x: timestamps,
        y: valueshum
    }];

const layoutHum = {
    xaxis: {
        title: '<b>Čas<b>',
        linecolor: 'black',
        linewidth: 2
    },
    yaxis: {
        title: '<b>% Vlhkost</b>',
        titlefont: {
            size: 14,
            color: '#000'
        },
        linecolor: 'black',
        linewidth: 2,
    },
    margin: {
        r: 50,
        pad: 0
    }
}
// At last we plot data :-)
Plotly.newPlot(myPlotHum, dataHum, layoutHum, { responsive: true });
});