import classes from "./MeterBar.module.css";

const MeterBar = ({ fillPercent , showPercent}) => {
   return (
      <div className={classes["meter-bar"]}>
         <div className={classes["bar-outer"]}>
            <div
               className={classes["bar-fill"]}
               style={{ width: fillPercent }}
            ><span>{fillPercent}</span></div>
         </div>
      </div>
   );
};

export default MeterBar;