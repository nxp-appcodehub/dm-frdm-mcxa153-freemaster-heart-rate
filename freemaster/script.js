/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

let ecgData = [];
let heartRateData = [];
let spo2Data = [];
let labels = [];

function createChart(ctx, data, label, borderColor, backgroundColor) {
    return new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [{
                label: label,
                data: data,
                borderColor: borderColor,
                backgroundColor: backgroundColor,
                fill: false,
                borderWidth: 2,
                lineTension: 0.1
            }]
        },
        options: {
			responsive: true,
			maintainAspectRatio: false,
            scales: {
                y: {
                    beginAtZero: false
                }
            },
            animation: {
                duration: 0
            },
            responsive: true,
            maintainAspectRatio: false
        }
    });
}

let hr_plot = createChart(document.getElementById('hr_plot').getContext('2d'), ecgData, 'ECG', 'green', 'rgba(0, 255, 0, 0.5)');
let spo2_plot = createChart(document.getElementById('spo2_plot').getContext('2d'), spo2Data, 'SPO2 (%)', 'red', 'rgba(255, 0, 0, 0.5)');

function on_error(err) {
    document.getElementById("status").innerHTML = err;
}

function addDataToChart(dataArr, chart, value, valueSpanId) {
    dataArr.push(value);

	if (valueSpanId === 'heartRateValue') {
		document.getElementById('heartRateDisplay').textContent = value + 'BPM';
	} 
	else if (valueSpanId === 'spo2Value') {
		document.getElementById('spo2Display').textContent = value + '%';
		document.getElementById(valueSpanId).innerText = value;
	}
	chart.update();

    if (dataArr.length > 100) { 
        dataArr.shift();
    }
}

function read_variable(name, valueSpanId) {
    if (pcm.ReadVariable(name)) {
        let value = pcm.LastVariable_vValue;
        if (name === 'heart_rate') {
            addDataToChart(heartRateData, hr_plot, value, valueSpanId);
        } 
		else if (name === 'spo2') {
            addDataToChart(spo2Data, spo2_plot, value, valueSpanId);
        }
		else if (name === 'g_ecg')
		{
			addDataToChart(ecgData, hr_plot, value, valueSpanId);
		}
        on_error("Ready");
    } 
	else {
        on_error("Error reading " + name + ": " + pcm.LastRetMsg);
    }
}

function updateValues() {
    labels.push(new Date().toLocaleTimeString());

    read_variable('heart_rate', 'heartRateValue');
    read_variable('spo2', 'spo2Value');
    read_variable('g_ecg', 'ecgValue');
	
    if (labels.length > 100) {
        labels.shift();
    }
}

setInterval(updateValues, 50);  
