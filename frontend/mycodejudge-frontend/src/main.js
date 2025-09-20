(function(){
  var editor = ace.edit("editor");
  editor.setTheme("ace/theme/monokai");
  editor.session.setMode("ace/mode/c_cpp");
  editor.session.setTabSize(2)
  editor.session.setUseSoftTabs(true);
  document.getElementById('editor').style.fontSize='16px';

  editor.setValue("#include <stdio.h>\nint main() {\n  return 0;\n}");

  var lang = "c";

  if (localStorage.getItem("problems") == null) {
    localStorage.setItem("problems", "[]");
  }

  if (localStorage.getItem("currentProblem") == null) {
    localStorage.setItem("currentProblem", "")
  }

  document.getElementById("run").onclick = function() {
    Swal.fire({
      title: "Please wait..."
    })
    axios.post("http://localhost:8000/post", {
      post: editor.getValue(),
      input: document.getElementById("text-input").value,
      lang: lang
    }).then(function (response){
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
    }).catch(function(err) {
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

    for(var i = 0; i < a.length; i++) {
      if(a[i] == " " || a[i] == "\t" || a[i] == "\n") {
        if (temp == "") {
          continue;
        }else {
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

    for(var i = 0; i < b.length; i++) {
      if(b[i] == " " || b[i] == "\t" || b[i] == "\n") {
        if (temp == "") {
          continue;
        }else {
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

    while(arr1[arr1.length - 1] == "\n") {
      arr1.pop();
    }

    while(arr2[arr2.length - 1] == "\n") {
      arr2.pop();
    }

    console.log(arr1);
    console.log(arr2);

    if (arr1.length != arr2.length) {
      return false;
    }

    for(var x = 0; x < arr1.length; x++) {
      if (arr1[x] != arr2[x]) {
        return false;
      }
    }
    return true;
  }

  document.getElementById("solve").onclick = function(e) {
    var index = parseInt(JSON.parse(localStorage.getItem("currentProblem")));
    console.log(index);
    var item = JSON.parse(localStorage.getItem("problems"))[index];
    console.log(item);
    axios.post("http://localhost:8000/post", {
      post: editor.getValue(),
      input: item.testcases,
      lang: lang
    }).then(function(r) {
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

  document.getElementById("problem-creator").style.display = "block";

  document.getElementById("login-form").onsubmit = function(e) {
    e.preventDefault();
  };

  document.getElementById("langc").onclick = function(e) {
    lang = "c";
    console.log("c");
    Swal.fire({
      text: "C"
    });
  };

  document.getElementById("langcpp").onclick = function(e) {
    lang = "cpp";
    console.log("cpp");
    Swal.fire({
      text: "C++"
    });
  }

  document.getElementById("mback").onclick = function(e) {
    document.getElementById("main-window").style.display = "none";
    document.getElementById("probList").style.display = "block";
  }

  document.getElementById("problemSubmit").onclick = function(e) {
    var arr = JSON.parse(localStorage.getItem("problems"));
    arr.push({
      title: document.getElementById("ptitle").value,
      desc: document.getElementById("pdesc").value,
      testcases: document.getElementById("ptestcases").value,
      answers: document.getElementById("panswers").value
    });
    localStorage.setItem("problems", JSON.stringify(arr));
    Swal.fire({
      text: "Done!"
    });
  }

  function gotoMainWindow(num) {
    return function() {
      localStorage.setItem("currentProblem", "" + num);
      var item = JSON.parse(localStorage.getItem("problems"))[num];
      document.getElementById("probList").style.display = "none";
      document.getElementById("main-window").style.display = "block";
      document.getElementById("mtitle").innerText = item.title;
      document.getElementById("mdesc").innerText = item.desc;
    };
  }

  function refreshProblemList() {
    var arr = JSON.parse(localStorage.getItem("problems"));
    document.getElementById("longlist").innerHTML = "";

    var masterButton = document.createElement("input");
    masterButton.type = "button";
    masterButton.onclick = function() {
      document.getElementById("probList").style.display = "none";
      document.getElementById("problem-creator").style.display = "block";
    }
    masterButton.value = "Back";
    document.getElementById("longlist").appendChild(masterButton);

    for (var i = 0; i < arr.length; i++) {
      var item = document.createElement("div");
      var d = document.createElement("p");
      d.innerText = arr[i].title;
      var btnEnter = document.createElement("input");
      btnEnter.type = "button";
      btnEnter.onclick = gotoMainWindow(i);
      btnEnter.value = "Go!";
      var btnDelete = document.createElement("input");
      btnDelete.type = "button";
      btnDelete.onclick = deleteAProblem(i);
      btnDelete.value = "Delete";
      item.appendChild(d);
      item.appendChild(btnEnter);
      item.appendChild(btnDelete);
      document.getElementById("longlist").appendChild(item);
    }
  }

  function deleteAProblem(num) {
    return function() {
      var arr = JSON.parse(localStorage.getItem("problems"));
      arr.splice(num, 1);
      localStorage.setItem("problems", JSON.stringify(arr));
      refreshProblemList();
    }
  }

  document.getElementById("gotoList").onclick = function(e) {
    document.getElementById("problem-creator").style.display = "none";
    document.getElementById("probList").style.display = "block";
    refreshProblemList();
  }
})();