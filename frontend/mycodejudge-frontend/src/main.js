(function () {
  var editor = ace.edit("editor");
  editor.setTheme("ace/theme/monokai");
  editor.session.setMode("ace/mode/c_cpp");
  editor.session.setTabSize(2)
  editor.session.setUseSoftTabs(true);
  document.getElementById('editor').style.fontSize = '16px';

  editor.setValue("#include <stdio.h>\nint main() {\n  return 0;\n}");

  var lang = "c";
  var currentProblemVar = null;

  document.getElementById("run").onclick = function () {
    Swal.fire({
      title: "Please wait..."
    })
    axios.post("http://localhost:8000/postsafe", {
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

  function matchAnswers(a, b) {
    var arr1 = [];
    var arr2 = [];
    var temp = "";

    for (var i = 0; i < a.length; i++) {
      if (a[i] == " " || a[i] == "\t" || a[i] == "\n") {
        if (temp == "") {
          continue;
        } else {
          arr1.push(temp);
          if (a[i] == "\n") {
            arr1.push("\n");
          }
          temp = "";
        }
      } else {
        temp += a[i];
      }
    }
    if (temp != "") { arr1.push(temp); }

    temp = "";

    for (var i = 0; i < b.length; i++) {
      if (b[i] == " " || b[i] == "\t" || b[i] == "\n") {
        if (temp == "") {
          continue;
        } else {
          arr2.push(temp);
          if (b[i] == "\n") {
            arr2.push("\n");
          }
          temp = "";
        }
      } else {
        temp += b[i];
      }
    }
    if (temp != "") { arr2.push(temp); }

    while (arr1[arr1.length - 1] == "\n") {
      arr1.pop();
    }

    while (arr2[arr2.length - 1] == "\n") {
      arr2.pop();
    }

    // console.log(arr1);
    // console.log(arr2);

    if (arr1.length != arr2.length) {
      return false;
    }

    for (var x = 0; x < arr1.length; x++) {
      if (arr1[x] != arr2[x]) {
        return false;
      }
    }
    return true;
  }

  document.getElementById("solve").onclick = function (e) {
    // var index = parseInt(JSON.parse(localStorage.getItem("currentProblem")));
    // console.log(index);
    // var item = JSON.parse(localStorage.getItem("currentProblem"));
    var item = currentProblemVar;
    // console.log(item);
    axios.post("http://localhost:8000/postsafe", {
      post: editor.getValue(),
      input: item.testcases,
      lang: lang
    }).then(function (r) {
      var data = r.data;
      var result = data.result;
      if (matchAnswers(result, item.answers)) {
        Swal.fire({
          text: "Passed!",
          icon: "success"
        });
      } else {
        Swal.fire({
          text: "Failed!",
          icon: "error"
        })
      }
    });
  }

  document.getElementById("login-form").onsubmit = function (e) {
    e.preventDefault();
    var username = document.getElementById("username").value;
    var password = document.getElementById("password").value;
    if (username == "" || password == "") {
      alert("Blank");
      return;
    }
    axios.post("http://localhost:8000/login", {
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

  document.getElementById("register-form").onsubmit = function (e) {
    e.preventDefault();
    var username = document.getElementById("r-username").value;
    var password = document.getElementById("r-password").value;
    if (username == "" || password == "") {
      alert("Blank");
      return;
    };
    axios.post("http://localhost:8000/register", {
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

  document.getElementById("langc").onclick = function (e) {
    lang = "c";
    console.log("c");
    Swal.fire({
      text: "C"
    });
  };

  document.getElementById("langcpp").onclick = function (e) {
    lang = "cpp";
    console.log("cpp");
    Swal.fire({
      text: "C++"
    });
  }

  document.getElementById("mback").onclick = function (e) {
    document.getElementById("main-div").style.display = "none";
    document.getElementById("probList").style.display = "block";
  }

  document.getElementById("problemSubmit").onclick = function (e) {
    axios.post("http://localhost:8000/postproblem", {
      admin: document.getElementById("admin-name").value,
      password: document.getElementById("admin-password").value,
      title: document.getElementById("ptitle").value,
      desc: document.getElementById("pdesc").value,
      testcases: document.getElementById("ptestcases").value,
      answers: document.getElementById("panswers").value
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

  function gotoMainWindow(obj) {
    return function () {
      // localStorage.setItem("currentProblem", JSON.stringify(obj));
      currentProblemVar = obj;
      document.getElementById("probList").style.display = "none";
      document.getElementById("main-div").style.display = "block";
      document.getElementById("mtitle").innerText = obj.title;
      document.getElementById("mdesc").innerText = obj.desc;
    };
  }

  function refreshProblemList() {
    // var arr = JSON.parse(localStorage.getItem("problems"));
    var username = localStorage.getItem("username");
    var sessionid = localStorage.getItem("sessionid");
    axios.post("http://localhost:8000/getproblems", {
      username,
      sessionid
    }).then(function (r) {
      if (r.data.status == "success") {
        // console.log(r.data);
        document.getElementById("longlist").innerHTML = "";

        for (var i = 0; i < r.data.list.length; i++) {
          // Create container as a Bootstrap card
          var item = document.createElement("div");
          item.className = "card shadow-sm mb-3";

          var cardBody = document.createElement("div");
          cardBody.className = "card-body d-flex justify-content-between align-items-center";

          // Title
          var d = document.createElement("p");
          d.className = "mb-0 fw-semibold"; // no margin + bold
          d.innerText = r.data.list[i].title;

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

  function deleteAProblem(num) {
    return function () {
      //var arr = JSON.parse(localStorage.getItem("problems"));
      var arr = getProblemsList();
      // arr.splice(num, 1);
      // localStorage.setItem("problems", JSON.stringify(arr));
      // refreshProblemList();
    }
  }

  document.getElementById("delete-problem-button").onclick = function (e) {
    axios.post("http://localhost:8000/deleteproblems", {
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

  function hideAll() {
    document.getElementById("main-div").style.display = "none";
    document.getElementById("login").style.display = "none";
    document.getElementById("register").style.display = "none";
    document.getElementById("problem-creator").style.display = "none";
    document.getElementById("probList").style.display = "none";
    document.getElementById("problem-deleter").style.display = "none";
  }

  function addLogoutButton() {
    var btn = document.createElement("input");
    btn.type = "button";
    btn.className = "btn btn-outline-light ms-auto"; // light button, aligned right
    btn.value = "🚪 Logout";
    btn.onclick = function () {
      axios.post("http://localhost:8000/logout", {
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

  var router = new Navigo("/");

  router.on("/", function () {
    hideAll();
    // Entry point
    if (localStorage.getItem("sessionid") != null) {
      document.getElementById("probList").style.display = "block";
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

  router.resolve();
})();