/*
 * Copyright (C) 2010-2011 Josh A. Beam
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

function PaintCanvas(userId, updateId, canvasId, controlsId, usersOnlineId)
{
	var m_canvas = document.getElementById(canvasId);
	var m_context = m_canvas.getContext("2d");
	var m_controls = document.getElementById(controlsId);
	var m_usersOnline = document.getElementById(usersOnlineId);

	var m_mouseDown = false;
	var m_mouseX = 0;
	var m_mouseY = 0;

	var m_lineData = "";

	var m_request = null;
	var m_parseUpdatesInterval = null;
	var m_postUpdateInterval = null;
	var m_lastUpdateId = updateId;

	var m_brushSize = 4;
	var m_brushColor = "#000000";

	function construct()
	{
		// just return if there's no canvas support in the browser
		if(m_context == null)
			return;

		// make the controls visible
		if(m_controls != null)
			m_controls.style.display = "block";

		m_canvas.onmousedown = mouseDown;
		m_canvas.onmousemove = mouseMove;

		var image = new Image();
		image.onload = function() {
			// show image on canvas
			m_context.drawImage(image, 0, 0);

			// open a connection to receive updates
			setTimeout(openConnection, 500);
		}
		var d = new Date();
		image.src = "Data/Canvas.png?t=" + d.getTime();
	}

	function getMouseX(e)
	{
		return (e.layerX == undefined) ? e.offsetX : e.layerX;
	}

	function getMouseY(e)
	{
		return (e.layerY == undefined) ? e.offsetY : e.layerY;
	}

	function mouseDown(e)
	{
		if(!e)
			e = window.event;

		document.onmouseup = mouseUp;
		document.onmouseout = mouseUp;

		m_mouseDown = true;
		m_mouseX = getMouseX(e);
		m_mouseY = getMouseY(e);

		queueLineData(m_mouseX, m_mouseY, m_mouseX, m_mouseY);
		drawLine(m_brushSize, m_brushColor, m_mouseX, m_mouseY, m_mouseX, m_mouseY);
	}

	function mouseUp(e)
	{
		if(!e)
			e = window.event;

		document.onmouseup = null;
		document.onmouseout = null;

		m_mouseDown = false;
	}

	function mouseMove(e)
	{
		if(!e)
			e = window.event;

		if(m_mouseDown == false)
			return;

		var x = getMouseX(e);
		var y = getMouseY(e);

		var xdiff = x - m_mouseX;
		var ydiff = y - m_mouseY;

		queueLineData(m_mouseX, m_mouseY, x, y);
		drawLine(m_brushSize, m_brushColor, m_mouseX, m_mouseY, x, y);
		m_mouseX = x;
		m_mouseY = y;
	}

	function parseLines(size, color, lines)
	{
		var l = lines.split(';');

		// draw each line
		for(var i = 0; i < l.length; ++i) {
			var coords = l[i].split(",");
			var x1 = parseInt(coords[0]);
			var y1 = parseInt(coords[1]);
			var x2 = parseInt(coords[2]);
			var y2 = parseInt(coords[3]);
			drawLine(size, color, x1, y1, x2, y2);
		}
	}

	function parseUpdate(update)
	{
		var u = update.split(' ');
		if(u.length == 4) {
			// if the update id is greater than the last
			// update id, continue parsing the update
			var thisUpdateId = parseInt(u[0]);
			if(thisUpdateId > m_lastUpdateId) {
				parseLines(u[1], u[2], u[3]);
				m_lastUpdateId = thisUpdateId;
			}
		}
	}

	function parseUpdates()
	{
		if(!m_request)
			return;

		var updates = m_request.responseText.split('\n');
		for(var i = 0; i < updates.length; ++i) {
			if(updates[i].indexOf("hi:") == 0) {
				// do nothing
			} else if(updates[i].indexOf("uo:") == 0) {
				m_usersOnline.innerHTML = "Users Online: " + parseInt(updates[i].substring(3));
			} else {
				parseUpdate(updates[i]);
			}
		}
	}

	function connectionStateChanged()
	{
		if(!m_request || m_request.readyState < 2)
			return;

		// just quit if a bad status code is given
		if(m_request.status != 200) {
			closeConnection();
			return;
		}

		// re-open the connection if it's closed
		if(m_request.readyState == 4) {
			closeConnection();
			setTimeout(openConnection, 500);
		}
	}

	function openConnection()
	{
		// just return if already connected
		if(m_request != null)
			return;

		var src = "PaintAction/GetUpdates";
		src += "?u=" + userId;
		src += "&i=" + m_lastUpdateId;

		// create request
		m_request = new XMLHttpRequest();
		m_request.open("GET", src, true);
		m_request.onreadystatechange = connectionStateChanged;
		m_request.send("t=" + (new Date()).getTime());

		// create intervals to parse/post updates
		if(m_parseUpdatesInterval == null)
			m_parseUpdatesInterval = setInterval(parseUpdates, 50);
		if(m_postUpdateInterval == null)
			m_postUpdateInterval = setInterval(postUpdate, 250);
	}

	function closeConnection()
	{
		// close the connection for receiving data
		if(m_request != null) {
			parseUpdates();
			if(m_request.readyState != 4)
				m_request.abort();
			m_request = null;
		}

		// clear the intervals
		if(m_parseUpdatesInterval != null) {
			clearInterval(m_parseUpdatesInterval);
			m_parseUpdatesInterval = null;
		}
		if(m_postUpdateInterval != null) {
			clearInterval(m_postUpdateInterval);
			m_postUpdateInterval = null;
		}
	}

	function postUpdate()
	{
		// just return if there's no data to be posted
		if(m_lineData.length == 0)
			return;

		// create request
		var request = new XMLHttpRequest();
		request.open("POST", "PaintAction/PostUpdate", true);

		var data = "?u=" + userId;
		data += "&s=" + m_brushSize;
		data += "&c=" + m_brushColor;
		data += "&l=" + m_lineData;

		// send data
		request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		request.send(data);
		m_lineData = "";
	}

	function queueLineData(x1, y1, x2, y2)
	{
		// add line data to queue
		if(m_lineData.length != 0)
			m_lineData += ";";
		m_lineData += x1 + "," + y1 + "," + x2 + "," + y2;
	}

	function drawPoint(size, x, y)
	{
		m_context.beginPath();
		m_context.arc(x, y, size / 2, 0, Math.PI * 2, true);
		m_context.closePath();
		m_context.fill();
	}

	function drawLine(size, color, x1, y1, x2, y2)
	{
		m_context.fillStyle = color;
		m_context.strokeStyle = color;

		// if the size of the line is 1 pixel,
		// just use the canvas lineTo function
		if(size == 1) {
			m_context.lineWidth = size;
			m_context.beginPath();
			m_context.moveTo(x1, y1);
			m_context.lineTo(x2, y2);
			m_context.closePath();
			m_context.stroke();
			return;
		}

		var xdiff = x2 - x1;
		var ydiff = y2 - y1;

		if(xdiff == 0 && ydiff == 0) {
			drawPoint(size, x1, y1);
			return;
		}

		if(Math.abs(xdiff) > Math.abs(ydiff)) {
			var xmin, xmax;

			// set xmin to the lower x value given
			// and xmax to the higher value
			if(x1 < x2) {
				xmin = x1;
				xmax = x2;
			} else {
				xmin = x2;
				xmax = x1;
			}

			// draw line in terms of y slope
			var slope = ydiff / xdiff;
			for(var x = xmin; x <= xmax; ++x) {
				var y = y1 + ((x - x1) * slope);
				drawPoint(size, x, y);
			}
		} else {
			var ymin, ymax;

			// set ymin to the lower y value given
			// and ymax to the higher value
			if(y1 < y2) {
				ymin = y1;
				ymax = y2;
			} else {
				ymin = y2;
				ymax = y1;
			}

			// draw line in terms of x slope
			var slope = xdiff / ydiff;
			for(var y = ymin; y <= ymax; ++y) {
				var x = x1 + ((y - y1) * slope);
				drawPoint(size, x, y);
			}
		}
	}

	this.drawLine = function(size, color, x1, y1, x2, y2)
	{
		drawLine(size, color, x1, y1, x2, y2);
	}

	this.setBrushSize = function(value)
	{
		postUpdate();
		m_brushSize = value;
	}

	this.setBrushColor = function(value)
	{
		postUpdate();
		m_brushColor = value;
	}

	construct();
}
