import classes from "./Button.module.css";

const Button = (props) => {
   return (
      <button
         className={`${classes.button} ${
            props.selected ? classes.selected : ""
         }`}
      onClick={props.onClick}>
         {props.children}
      </button>
   );
};

export default Button;
