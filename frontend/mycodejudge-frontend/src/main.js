(function(){
  var editor = ace.edit("editor");
  editor.setTheme("ace/theme/monokai");
  editor.session.setMode("ace/mode/c_cpp");
  editor.session.setTabSize(2)
  editor.session.setUseSoftTabs(true);
  document.getElementById('editor').style.fontSize='16px';

  var lang = "c";

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
      Swal.fire({
        title: "Error/s:",
        text: response.data.errors
      })
    }).catch(function(err) {
      Swal.fire({
        title: "Error!",
        icon: "error",
        text: "error"
      })
    });
  };

  document.getElementById("main-window").style.display = "block";

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

})();