import axios from "axios";
import { marked } from "marked";
import Navigo from "navigo";
import Swal from "sweetalert2";
import DOMPurify from "dompurify";
// import {} from "bootstrap";

var location = "";
if (window.location.hostname.indexOf("localhost") > -1 || window.location.hostname.indexOf("192.168.") > -1) {
  location = window.location.protocol + "//" + window.location.hostname + ":3000";
} else {
  location = window.location.protocol + "//" + window.location.hostname;
}

axios.get(location + "/api/status").then(function (r) {
  console.log(r.data);
}).catch(function(e) {
  Swal.fire({
    icon: "error",
    title: "Error!",
    text: "Cannot reach the backend!"
  });
});

var editor = ace.edit("editor");
editor.setTheme("ace/theme/monokai");
editor.session.setMode("ace/mode/c_cpp");
editor.session.setTabSize(2)
editor.session.setUseSoftTabs(true);
document.getElementById('editor').style.fontSize = '16px';

editor.setValue("#include <stdio.h>\nint main() {\n  return 0;\n}");


var pgEditor = ace.edit("pg-editor");
pgEditor.setTheme("ace/theme/monokai");
pgEditor.session.setMode("ace/mode/c_cpp");
pgEditor.session.setTabSize(2);
pgEditor.session.setUseSoftTabs(true);
document.getElementById("pg-editor").style.fontSize = '16px';

var lang = "c";
var currentProblemVar = null;
var currentProblemSolved = false;

var testCasesCount = 0;
var testCasesObj = {};

var langcelm = document.getElementsByClassName("langc");
var langcppelm = document.getElementsByClassName("langcpp");
var langjavaelm = document.getElementsByClassName("langjava");

/* Run button from problem page */
document.getElementById("run").onclick = function () {
  Swal.fire({
    title: "Please wait...",
    showConfirmButton: false,
    showCancelButton: false
  })
  axios.post(location + "/api/postsafe", {
    post: editor.getValue(),
    input: document.getElementById("text-input").value,
    lang: lang
  }).then(function (response) {
    console.log(response.data);
    document.getElementById("output").value = response.data.result;
    if (response.data.errors != "") {
      Swal.fire({
        title: "Error/s:",
        text: response.data.errors
      });
    } else {
      Swal.fire({
        title: "Success!",
        icon: "success"
      })
    }
  }).catch(function (err) {
    Swal.fire({
      title: "Error!",
      icon: "error",
      text: "error"
    })
  });
};

/* Run button from playground*/
document.getElementById("pg-run").onclick = function () {
  Swal.fire({
    title: "Please wait...",
    showConfirmButton: false,
    showCancelButton: false
  })
  axios.post(location + "/api/postsafe", {
    post: pgEditor.getValue(),
    input: document.getElementById("pg-text-input").value,
    lang: lang
  }).then(function (response) {
    console.log(response.data);
    document.getElementById("pg-output").value = response.data.result;
    if (response.data.errors != "") {
      Swal.fire({
        title: "Error/s:",
        text: response.data.errors
      });
    } else {
      Swal.fire({
        title: "Success!",
        icon: "success"
      })
    }
  }).catch(function (err) {
    Swal.fire({
      title: "Error!",
      icon: "error",
      text: "error"
    })
  });
};

/* Submit button from problem page */
document.getElementById("solve").onclick = function (e) {
  var item = currentProblemVar;
  currentProblemSolved = false;
  Swal.fire({
    text: "Please wait...",
    showConfirmButton: false,
    showCancelButton: false
  });
  axios.post(location + "/api/submitcheck", {
    username: localStorage.getItem("username"),
    sessionid: localStorage.getItem("sessionid"),
    title: item.title,
    code: editor.getValue(),
    lang: lang
  }).then(function (r) {
    var data = r.data;
    if (data.status == "success") {
      Swal.fire({
        icon: "success",
        text: "Solution accepted!"
      })
    } else {
      Swal.fire({
        icon: "error",
        text: "Solution not accepted: " + data.message
      })
    }
  });
}

/* Log In submit event from LogIn page */
document.getElementById("login-form").onsubmit = function (e) {
  e.preventDefault();
  var username = document.getElementById("username").value;
  var password = document.getElementById("password").value;
  if (username == "" || password == "") {
    alert("Blank");
    return;
  }
  // alert(location + "/login");
  axios.post(location + "/api/login", {
    username,
    password
  }).then(function (r) {
    if (r.data.status == "success") {
      localStorage.setItem("username", username);
      localStorage.setItem("sessionid", r.data.sessionid);
      window.location.reload();
    } else {
      alert("login failed...");
    }
    console.log(r);
  });
};

/* Register event from Register page */
document.getElementById("register-form").onsubmit = function (e) {
  e.preventDefault();
  var username = document.getElementById("r-username").value;
  var password = document.getElementById("r-password").value;
  if (username == "" || password == "") {
    alert("Blank");
    return;
  };
  axios.post(location + "/api/register", {
    username,
    password
  }).then(function (r) {
    if (r.data.status == "success") {
      alert("Registered");
    } else {
      alert("register failed...");
    }
    console.log(r);
  });
}

document.getElementById("switch-to-login").onclick = function () {
  document.getElementById("register").style.display = "none";
  document.getElementById("login").style.display = "block";
}

document.getElementById("switch-to-register").onclick = function () {
  document.getElementById("login").style.display = "none";
  document.getElementById("register").style.display = "block";
}

/* Handles button clicks for changing the editor programming language/s */

for (var i = 0; i < langcelm.length; i++) {
  langcelm[i].onclick = function (e) {
    lang = "c";
    console.log("c");
    Swal.fire({
      text: "C"
    });
    editor.session.setMode("ace/mode/c_cpp");
    var titles = document.getElementsByClassName("code-editor-title");
    for (var j = 0; j < titles.length; j++) {
      titles[j].innerText = "C Code Editor";
    }
  };
}

for (var i = 0; i < langcppelm.length; i++) {
  langcppelm[i].onclick = function (e) {
    lang = "cpp";
    console.log("cpp");
    Swal.fire({
      text: "C++"
    });
    editor.session.setMode("ace/mode/c_cpp");
    var titles = document.getElementsByClassName("code-editor-title");
    for (var j = 0; j < titles.length; j++) {
      titles[j].innerText = "C++ Code Editor";
    }
  }
}

for (var i = 0; i < langjavaelm.length; i++) {
  langjavaelm[i].onclick = function (e) {
    lang = "java";
    console.log("java");
    Swal.fire({
      text: "Java"
    });
    editor.session.setMode("ace/mode/java");
    var titles = document.getElementsByClassName("code-editor-title");
    for (var j = 0; j < titles.length; j++) {
      titles[j].innerText = "Java Code Editor";
    }
  }
}

/* This handles the button click to go back on the problem list */
document.getElementById("mback").onclick = function (e) {
  document.getElementById("main-div").style.display = "none";
  document.getElementById("probList").style.display = "block";
}

/* This event submits the problem to the backend */
document.getElementById("problemSubmit").onclick = function (e) {
  testCasesObj["length"] = testCasesCount;
  testCasesObj["items"] = [];
  var i;
  for (i = 0; i < testCasesCount; i++) {
    testCasesObj["items"].push({
      "test": document.getElementById("test-" + i).value,
      "answer": document.getElementById("answer-"+i).value
    });
  }
  
  axios.post(location + "/api/postproblem", {
    admin: document.getElementById("admin-name").value,
    password: document.getElementById("admin-password").value,
    title: document.getElementById("ptitle").value,
    desc: document.getElementById("pdesc").value,
    testcases: JSON.stringify(testCasesObj),
    answers: ""
  }).then(function (r) {
    if (r.data.status == "success") {
      Swal.fire({
        text: "Done!"
      });
    } else {
      Swal.fire({
        text: "Error posting problem"
      });
    }
  });
}

/* Show code from list of codes */
function showCode(code) {
  return function() {
    Swal.fire({
      html: '<div id="temp-code-view" style="height: 200px"></div>',
      didOpen: function() {
        var editor = ace.edit("temp-code-view");
        editor.setTheme("ace/theme/monokai");
        editor.session.setMode("ace/mode/c_cpp");
        editor.session.setTabSize(2)
        editor.session.setUseSoftTabs(true);
        document.getElementById('temp-code-view').style.fontSize = '12px';
        editor.setValue(code);
        // alert(code);
      }
    });
  };
}

/* Show list of submitted codes */
function showAdminViewLists(arr) {
  var list = document.getElementById("view-list");
  list.innerHTML = ""; // clear old content

  for (var i = 0; i < arr.length; i++) {
    // Outer card
    var el = document.createElement("div");
    el.className = "card mb-3 shadow-sm";

    // Card body
    var cardBody = document.createElement("div");
    cardBody.className = "card-body d-flex justify-content-between align-items-center";

    // Info section
    var infoDiv = document.createElement("div");

    var elName = document.createElement("h6");
    elName.className = "card-subtitle mb-1 text-muted";
    elName.innerText = "👤 " + arr[i].username;

    var elTitle = document.createElement("h5");
    elTitle.className = "card-title mb-1";
    // elTitle.innerText = arr[i].title;
    elTitle.innerHTML = DOMPurify.sanitize(marked.parseInline(arr[i].title));

    var elDate = document.createElement("small");
    elDate.className = "text-secondary";
    elDate.innerText =
      "📅 " + new Date(parseInt(arr[i].submitdate) * 1000).toLocaleString();

    infoDiv.appendChild(elTitle);
    infoDiv.appendChild(elName);
    infoDiv.appendChild(elDate);

    // Button
    var btn = document.createElement("button");
    btn.className = "btn btn-sm btn-outline-primary";
    btn.innerText = "View Solution";
    btn.onclick = showCode(arr[i].solution);

    // Put together
    cardBody.appendChild(infoDiv);
    cardBody.appendChild(btn);
    el.appendChild(cardBody);
    list.appendChild(el);
  }
}

/* Handle click event to fetch list of submitted codes */
document.getElementById("v-submit").onclick = function (e) {
  axios.post(location + "/api/view", {
    admin: document.getElementById("v-admin").value,
    password: document.getElementById("v-pass").value,
  }).then(function(r) {
    if (r.data.status == "success") {
      showAdminViewLists(r.data.list);
    } else {
      alert("Failed to fetch admin lists");
    }
  });
}

document.getElementById("pg").onclick = playground;

/* Exit playground */
document.getElementById("pg-mback").onclick = function () {
  hideAll();
  window.location.reload();
}

/* Main page to show, includes the code editor and problem */
function gotoMainWindow(obj) {
  return function () {
    currentProblemVar = obj;
    document.getElementById("probList").style.display = "none";
    document.getElementById("main-div").style.display = "block";
    document.getElementById("mtitle").innerHTML = DOMPurify.sanitize(marked.parse(obj.title));
    document.getElementById("mdesc").innerHTML = DOMPurify.sanitize(marked.parse(obj.desc));
  };
}

/* Load or refresh problem list */
function refreshProblemList() {
  var username = localStorage.getItem("username");
  var sessionid = localStorage.getItem("sessionid");
  axios.post(location + "/api/getproblems", {
    username,
    sessionid
  }).then(function (r) {
    if (r.data.status == "success") {
      document.getElementById("longlist").innerHTML = "";

      for (var i = 0; i < r.data.list.length; i++) {
        var item = document.createElement("div");
        item.className = "card shadow-sm mb-3";

        var cardBody = document.createElement("div");
        cardBody.className = "card-body d-flex justify-content-between align-items-center";

        // Title
        var d = document.createElement("p");
        d.className = "mb-0 fw-semibold"; // no margin + bold
        d.innerHTML = DOMPurify.sanitize(marked.parseInline(r.data.list[i].title));

        // Button
        var btnEnter = document.createElement("button");
        btnEnter.className = "btn btn-primary btn-sm"; // Bootstrap-styled button
        btnEnter.innerText = "Go!";
        btnEnter.onclick = gotoMainWindow(r.data.list[i]);

        // Assemble
        cardBody.appendChild(d);
        cardBody.appendChild(btnEnter);
        item.appendChild(cardBody);

        // Add to list
        document.getElementById("longlist").appendChild(item);
      }
    } else {
      alert("refresh failed...");
    }
  });
}

/* Delete all problems */ 
document.getElementById("delete-problem-button").onclick = function (e) {
  axios.post(location + "/api/deleteproblems", {
    admin: document.getElementById("d-admin-name").value,
    password: document.getElementById("d-admin-pass").value
  }).then(function (r) {
    if (r.data.status == "success") {
      alert("All problems deleted");
    } else {
      alert("Cannot delete all problems");
    }
  })
}

/* Add test cases on creating a problem */
document.getElementById("addTestCases").onclick = function(e) {
  var parentElem = document.getElementById("pc-items");
  var curElem = document.createElement("div");
  var testCaseTestBox = document.createElement("textarea");
  var testCaseAnswerBox = document.createElement("textarea");
  
  testCaseTestBox.id = "test-" + testCasesCount;
  testCaseAnswerBox.id = "answer-" + testCasesCount;
  
  testCaseTestBox.className = "col-md-6 font-monospace";
  testCaseTestBox.placeholder = "Enter TestCase Input here...";
  testCaseTestBox.rows = 5;
  
  testCaseAnswerBox.className = "col-md-6 font-monospace";
  testCaseAnswerBox.placeholder = "Enter TestCase Answer here...";
  testCaseAnswerBox.rows = 5;
  
  testCasesCount++;
  
  curElem.appendChild(testCaseTestBox);
  curElem.appendChild(testCaseAnswerBox);
  parentElem.appendChild(curElem);
  
}
/* Hide all views */
function hideAll() {
  document.getElementById("main-div").style.display = "none";
  document.getElementById("login").style.display = "none";
  document.getElementById("register").style.display = "none";
  document.getElementById("problem-creator").style.display = "none";
  document.getElementById("probList").style.display = "none";
  document.getElementById("problem-deleter").style.display = "none";
  document.getElementById("submissions").style.display = "none";
  document.getElementById("view").style.display = "none";
  document.getElementById("playground").style.display = "none";
}

/* Show playground */
function playground() {
  hideAll();
  document.getElementById("playground").style.display = "block";
  var titles = document.getElementsByClassName("code-editor-title");
  for (var j = 0; j < titles.length; j++) {
    titles[j].innerText = "C Code Editor";
  }
}

/* Show Log Out button */
function addLogoutButton() {
  var btn = document.createElement("input");
  btn.type = "button";
  btn.className = "btn btn-outline-light ms-auto"; // light button, aligned right
  btn.value = "🚪 Logout";
  btn.onclick = function () {
    axios.post(location + "/api/logout", {
      username: localStorage.getItem("username"),
      sessionid: localStorage.getItem("sessionid")
    }).then(function (r) {
      localStorage.removeItem("username");
      localStorage.removeItem("sessionid");
      hideAll();
      document.getElementById("login").style.display = "block";
      window.location.reload();
    });
  }
  document.getElementById("navigation-bar").appendChild(btn);
  // document.body.appendChild(btn);
}

/* Show submissions lists (Non-Admin) */
function showSubmissions() {
  var elemList = document.getElementById("submitLists");
  elemList.innerHTML = ""; // clear old entries

  axios.get(location + "/api/getsubmissions").then(function (r) {
    for (var i = 0; i < r.data.list.length; i++) {
      var submission = r.data.list[i];

      var card = document.createElement("div");
      card.className = "card mb-2 shadow-sm";

      var cardBody = document.createElement("div");
      cardBody.className = "card-body d-flex justify-content-between align-items-center";

      var eName = document.createElement("span");
      eName.className = "fw-bold text-primary";
      eName.innerText = submission.username;

      var eTitle = document.createElement("span");
      eTitle.className = "text-dark mx-3 flex-grow-1";
      eTitle.innerHTML = DOMPurify.sanitize(marked.parseInline(submission.title));

      var eDate = document.createElement("span");
      eDate.className = "text-muted small";
      eDate.innerText = new Date(parseInt(submission.submitdate) * 1000).toLocaleString();

      cardBody.appendChild(eName);
      cardBody.appendChild(eTitle);
      cardBody.appendChild(eDate);

      card.appendChild(cardBody);
      elemList.appendChild(card);
    }
  });
}

var router = new Navigo("/");

router.on("/", function () {
  hideAll();

  // Entry point
  if (localStorage.getItem("sessionid") != null) {
    document.getElementById("probList").style.display = "block";
    var titles = document.getElementsByClassName("code-editor-title");
    for (var j = 0; j < titles.length; j++) {
      titles[j].innerText = "C Code Editor";
    }
    refreshProblemList();
    addLogoutButton();
  } else {
    document.getElementById("login").style.display = "block";
  }
});

router.on("/create", function () {
  hideAll();
  document.getElementById("problem-creator").style.display = "block";
});

router.on("/delete", function () {
  hideAll();
  document.getElementById("problem-deleter").style.display = "block";
});

router.on("/submissions", function () {
  hideAll();
  document.getElementById("submissions").style.display = "block";
  showSubmissions();
});

router.on("/view", function () {
  hideAll();
  document.getElementById("view").style.display = "block";
});

router.notFound(function () {
  Swal.fire({
    icon: "error",
    text: "Page does not exist!"
  })
});

router.resolve();
